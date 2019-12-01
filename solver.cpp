#include "solver.hpp"

#include <algorithm>
#include <set>

bool compare(Implicant &a, Implicant &b) {
    return a.num_ones < b.num_ones;
}

Solver::Solver(Table *table) : table(table) {
    for (unsigned int i = 0; i < table->rows; i++) {
        if (table->truth_table[i][table->numvars] == '1') {
            std::string impl_str = table->truth_table[i].substr(
                0, table->truth_table[i].length() - 1);
            impls.insert(impl_str);
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

bool Solver::done() const {
    for (auto const &impl : impls) {
        if (!impl.done) {
            return false;
        }
    }
    return true;
}

// Returns modified implicant (with '-' in place of  don't cares)
// if the comparison succeeds, or implicant a if the comparison fails,
// i.e. the implicants differ by more than one bit.
Implicant Solver::compare(const Implicant &a, const Implicant &b) const {
    std::string bin_rep(a.bin_rep.length(), ' ');

    for (unsigned int i = 0; i < a.bin_rep.length(); i++) {

        // Only implicants with dashes in line will form a valid comparison.
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
        return Implicant(bin_rep);
    }

    return a;
}

// A naive implementation of the Quine-McCluskey tabular method
// for minimizing boolean expressions:
// https://en.wikipedia.org/wiki/Quine-McCluskey_algorithm
std::set<Implicant> Solver::solve() {
    std::set<Implicant> result, used;

    while (!done()) {

        used = impls;

        for (auto orig : impls) {
            for (auto comp : impls) {

                if (orig.num_ones + 1 != comp.num_ones) {
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

        // any unused implicants are essential primes
        for (auto unused : used) {
            unused.done = true;
            result.insert(unused);
        }

        impls = result;
        result.clear();
    }

    return impls;
}
