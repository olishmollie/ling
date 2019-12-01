#ifndef _SOLVER_H
#define _SOLVER_H

#include "implicant.hpp"
#include "table.hpp"

#include <vector>
#include <set>

class Solver {
    Table *table;
    std::set<Implicant> impls;

    Implicant compare(const Implicant &a, const Implicant &b) const;
    bool done() const;
    void print_impls() const;

  public:
    Solver(Table *table);
    std::vector<Implicant> solve();
};

#endif
