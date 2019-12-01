#ifndef _SOLVER_H
#define _SOLVER_H

#include "implicant.hpp"
#include "table.hpp"

#include <vector>
#include <set>

class Solver {
    Table *table;
    std::set<Implicant> impls;

    Implicant compare(const Implicant &a, const Implicant &b);
    void solve();
    void print_set() const;

  public:
    Solver(Table *table);
    void run();
};

#endif
