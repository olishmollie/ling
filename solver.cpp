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

void Solver::print_set() const {
    if (impls.empty()) {
        std::cout << "empty" << std::endl;
    } else {
        for (auto const &impl : impls) {
            std::cout << impl.to_string() << std::endl;
        }
    }
    std::cout << std::endl;
}

bool done(std::set<Implicant> &impls) {
    for (auto const &impl : impls) {
        if (!impl.done) {
            return false;
        }
    }
    return true;
}

void Solver::run() {
    print_set();
    while (!done(impls)) {
        solve();
        print_set();
    }
}

Implicant Solver::compare(const Implicant &a, const Implicant &b) {
    std::string res(a.bin_rep.length(), ' ');

    for (unsigned int i = 0; i < a.bin_rep.length(); i++) {
        if (a.bin_rep[i] == '-') {
            if (b.bin_rep[i] != '-') {
                return a;
            }
            continue;
        }
        bool x = a.bin_rep[i] == '1';
        bool y = b.bin_rep[i] == '1';
        res[i] = (x ^ y) + '0';
    }

    if (std::count(res.begin(), res.end(), '1') == 1) {
        unsigned int idx = res.find('1');
        res = a.bin_rep;
        res[idx] = '-';
        return res;
    }

    return a;
}

void Solver::solve() {
    std::set<Implicant> result, used;

    used = impls;

    for (auto orig : impls) {
        for (auto comp : impls) {

            if (orig.num_ones + 1 != comp.num_ones) {
                continue;
            }

            Implicant next = compare(orig, comp);
            std::cout << "orig: " << orig.to_string()
                      << ", comp: " << comp.to_string()
                      << ", next: " << next.to_string() << std::endl;

            if (next != orig) {
                result.insert(next);
                used.erase(orig);
                used.erase(comp);
            }
        }
    }

    for (auto unused : used) {
        unused.done = true;
        result.insert(unused);
    }

    impls = result;
}
