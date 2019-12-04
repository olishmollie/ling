#ifndef _SOLVER_H
#define _SOLVER_H

#include "ast.hpp"
#include "implicant.hpp"
#include "table.hpp"

struct Solver {
    Table *table;

    std::set<Implicant> implicants;
    std::vector<unsigned int> minterms;
};

Solver new_solver(Table *table);
Ast *solve(Solver &self);

void print_implicants(const Solver &self);

#endif
