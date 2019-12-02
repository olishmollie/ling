#include "solver.hpp"

#include <algorithm>
#include <iomanip>

Solver::Solver(Table *table) : table(table) {
    for (unsigned int i = 0; i < table->rows; i++) {
        if (table->truth_table[i][table->numvars] == '1') {
            minterms.push_back(i);
            std::string impl_str = table->truth_table[i].substr(
                0, table->truth_table[i].length() - 1);
            Implicant impl = Implicant(impl_str);
            impl.add_minterm(i);
            impls.insert(impl);
        }
    }
}

void Solver::print_impls() const {
    if (impls.empty()) {
        std::cout << "empty" << std::endl;
    } else {
        for (auto const &impl : impls) {
            std::cout << impl.to_string() << std::endl;
        }
    }
    std::cout << std::endl;
}

// Returns modified implicant (with '-' in place of  don't cares)
// if the comparison succeeds, or implicant a if the comparison fails,
// i.e. the implicants differ by more than one bit.
Implicant compare(const Implicant &a, const Implicant &b) {
    std::string bin_rep(a.bin_rep.length(), ' ');

    for (unsigned int i = 0; i < a.bin_rep.length(); i++) {

        // Only implicants with dashes in line will
        // form a valid comparison.
        if (a.bin_rep[i] == '-') {
            if (b.bin_rep[i] != '-') {
                return a;
            }
            continue;
        }

        bool x = a.bin_rep[i] == '1';
        bool y = b.bin_rep[i] == '1';

        // XOR will yield a bitstring with a single 1
        // if the implicants differ by one bit. The add
        // turns the bool back into a char.
        bin_rep[i] = (x ^ y) + '0';
    }

    // Here's where we count the 1s. There's probably a more efficient
    // way to accomplish this. Perhaps a bitset or a vector<bool>? But we
    // somehow need three states: true, false, and '-' for don't care.
    if (std::count(bin_rep.begin(), bin_rep.end(), '1') == 1) {
        unsigned int idx = bin_rep.find('1');
        bin_rep = a.bin_rep;
        bin_rep[idx] = '-';

        // Add the prime_implicants of a and b.
        Implicant impl = Implicant(bin_rep);
        impl.add_minterms(a.minterms);
        impl.add_minterms(b.minterms);

        return impl;
    }

    return a;
}

bool cmp_num_ones(const Implicant &a, const Implicant &b) {
    return a.num_ones() < b.num_ones();
}

bool done(std::set<Implicant> &impls) {
    for (auto const &impl : impls) {
        if (!impl.done) {
            return false;
        }
    }
    return true;
}

std::vector<Implicant> generate_prime_implicants(std::set<Implicant> &impls) {
    std::vector<Implicant> prime_implicants;
    std::set<Implicant> result, used;

    while (!done(impls)) {

        used = impls;

        for (auto orig : impls) {
            for (auto comp : impls) {

                if (orig.num_ones() + 1 != comp.num_ones()) {
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

        impls = result;
        result.clear();
    }

    // Use the set to create a vector and sort it by number of ones. I
    // was trying to figure out to do this with a set alone, but
    // this is a lot easier.
    prime_implicants = std::vector<Implicant>(impls.begin(), impls.end());
    std::sort(prime_implicants.begin(), prime_implicants.end(), cmp_num_ones);

    return prime_implicants;
}

// An implementation of the Quine-McCluskey tabular method
// for minimizing boolean expressions:
// https://en.wikipedia.org/wiki/Quine-McCluskey_algorithm
Ast *Solver::solve() {
    unsigned int start = 0;
    std::string input;
    TokenVec tokens;
    std::vector<Implicant> prime_implicants;

    prime_implicants = generate_prime_implicants(impls);

    // Tautologies and condtradictions
    if (prime_implicants.empty()) {
        input = "F";
        tokens = tokenize(input);
        return parse(tokens, start);
    } else if (prime_implicants.size() == 1 && prime_implicants[0].is_true()) {
        input = "T";
        tokens = tokenize(input);
        return parse(tokens, start);
    }

    // This section generates an AST from the prime implicants found above.
    std::string *vars = table->get_vars();

    for (unsigned int i = 0; i < prime_implicants.size(); i++) {
        Implicant impl = prime_implicants[i];

        // If a 0 or 1 is found, we grab that variable.
        size_t idx = impl.bin_rep.find_first_of("01");
        while (idx != std::string::npos) {
            input.append(vars[idx]);

            // A zero negates the variable
            if (impl.bin_rep[idx] == '0') {
                input.append("'");
            }

            idx = impl.bin_rep.find_first_of("01", idx + 1);

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
