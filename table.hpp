#ifndef _TABLE_H
#define _TABLE_H

#include "implicant.hpp"
#include "parse.hpp"

struct Table {
    Ast *ast;
    Context ctx;

    unsigned int rows, cols;

    std::vector<std::string> vars;
    std::vector<std::string> data;

    ~Table() { delete ast; }
};

Table new_table(Ast *ast);
void delete_table(Table *self);

char get_output(const Table &self, const unsigned int row);
void solve(Table &self);

void print_table(Table &table);

#endif
