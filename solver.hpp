#ifndef _SOLVER_H
#define _SOLVER_H

#include "implicant.hpp"
#include "table.hpp"

#include <set>
#include <vector>

class Solver {
    Table *table;

    std::set<Implicant> impls;
    std::vector<unsigned int> minterms;

    bool **prime_table;

    Implicant compare(const Implicant &a, const Implicant &b) const;
    bool done() const;
    void print_impls() const;

  public:
    Solver(Table *table);
    ~Solver();

    std::vector<Implicant> prime_implicants;

    void solve();
    void petricks_method();
    void print_prime_table();
};

#endif
