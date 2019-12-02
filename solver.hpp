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

    void print_impls() const;

  public:
    Solver(Table *table);

    Ast *solve();
};

#endif
