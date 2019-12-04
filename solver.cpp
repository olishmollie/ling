#include "solver.hpp"

#include <algorithm>
#include <iomanip>
#include <set>
#include <vector>

using std::count;
using std::cout;
using std::endl;
using std::set;
using std::string;
using std::vector;

Solver new_solver(Table *table) {
    Solver solver;
    solver.table = table;
    return solver;
}

void init(Solver &self) {
    string bit_str;
    Implicant implicant;

    for (unsigned int i = 0; i < self.table->rows; i++) {
        if (get_output(*self.table, i) == '1') {
            self.minterms.push_back(i);
            bit_str =
                self.table->data[i].substr(0, self.table->data[i].length() - 1);
            implicant = new_implicant(bit_str);
            add_minterm(implicant, i);
            self.implicants.insert(implicant);
        }
    }
}

void print_implicants(const Solver &self) {
    if (self.implicants.empty()) {
        cout << "empty" << endl;
    } else {
        for (const Implicant implicant : self.implicants) {
            cout << to_string(implicant) << endl;
        }
    }
    cout << endl;
}

// Returns modified implicant (with '-' in place of  don't cares)
// if the comparison succeeds, or implicant a if the comparison fails,
// i.e. the implicants differ by more than one bit.
Implicant compare(const Implicant &a, const Implicant &b) {
    string bit_str(a.bit_str.length(), ' ');

    for (unsigned int i = 0; i < a.bit_str.length(); i++) {

        // Only implicants with dashes in line will
        // form a valid comparison.
        if (a.bit_str[i] == '-') {
            if (b.bit_str[i] != '-') {
                return a;
            }
            continue;
        }

        bool x = a.bit_str[i] == '1';
        bool y = b.bit_str[i] == '1';

        // XOR will yield a bit string with a single 1
        // if the implicants differ by one bit. The add
        // turns the bool back into a char.
        bit_str[i] = (x ^ y) + '0';
    }

    // Here's where we count the 1s. There's probably a more efficient
    // way to accomplish this. Perhaps a bitset or a vector<bool>? But we
    // somehow need three states: true, false, and '-' for don't care.
    if (count(bit_str.begin(), bit_str.end(), '1') == 1) {
        unsigned int idx = bit_str.find('1');

        // Make a new bit string, replacing the bit that differs
        // with a '-'.
        bit_str = a.bit_str;
        bit_str[idx] = '-';

        // Add the prime_implicants of a and b to the modified implicant.
        Implicant implicant = new_implicant(bit_str);
        add_minterms(implicant, a.minterms);
        add_minterms(implicant, b.minterms);

        return implicant;
    }

    // If they differ by more than one bit, return a.
    return a;
}

bool done(const set<Implicant> &implicants) {
    for (auto const &impl : implicants) {
        if (!impl.done) {
            return false;
        }
    }
    return true;
}

bool cmp_num_ones(const Implicant &a, const Implicant &b) {
    return num_ones(a) < num_ones(b);
}

std::vector<Implicant> generate_prime_implicants(Solver &self) {
    vector<Implicant> prime_implicants;
    set<Implicant> result, used;

    while (!done(self.implicants)) {

        used = self.implicants;

        for (auto orig : self.implicants) {
            for (auto comp : self.implicants) {

                if (num_ones(orig) + 1 != num_ones(comp)) {
                    continue;
                }

                Implicant next = compare(orig, comp);

                if (next != orig) {
                    result.insert(next);
                    used.erase(orig);
                    used.erase(comp);
                }
            }
        }

        // any unused implicants are essential prime_implicants
        for (auto unused : used) {
            unused.done = true;
            result.insert(unused);
        }

        self.implicants = result;

        result.clear();
    }

    prime_implicants =
        vector<Implicant>(self.implicants.begin(), self.implicants.end());
    std::sort(prime_implicants.begin(), prime_implicants.end(), cmp_num_ones);

    return prime_implicants;
}

// An implementation of the Quine-McCluskey tabular method
// for minimizing boolean expressions:
// https://en.wikipedia.org/wiki/Quine-McCluskey_algorithm
Ast *solve(Solver &self) {
    vector<Implicant> prime_implicants;

    unsigned int start = 0;
    string input;
    TokenVec tokens;

    init(self);
    prime_implicants = generate_prime_implicants(self);

    // Tautologies and condtradictions
    if (prime_implicants.empty()) {
        input = "F";
        tokens = tokenize(input);
        return parse(tokens, start);
    } else if (prime_implicants.size() == 1 && is_true(prime_implicants[0])) {
        input = "T";
        tokens = tokenize(input);
        return parse(tokens, start);
    }

    // This section generates an AST from the prime implicants found above.
    for (unsigned int i = 0; i < prime_implicants.size(); i++) {
        Implicant impl = prime_implicants[i];

        // If a 0 or 1 is found, we grab that variable.
        size_t idx = impl.bit_str.find_first_of("01");
        while (idx != std::string::npos) {
            input.append(self.table->vars[idx]);

            // A zero negates the variable
            if (impl.bit_str[idx] == '0') {
                input.append("'");
            }

            idx = impl.bit_str.find_first_of("01", idx + 1);

            // If it's not the last variable, needs a conjunction.
            if (idx != std::string::npos) {
                input.append("*");
            }
        }

        // If it's not the last implicant, needs a disjunction.
        if (i != prime_implicants.size() - 1) {
            input.append("+");
        }
    }

    // Feels a little weird to use the parser to reparse an input string,
    // but it's also kinda cool.
    tokens = tokenize(input);

    return parse(tokens, start);
}
