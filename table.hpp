#ifndef _TABLE_H
#define _TABLE_H

#include "implicant.hpp"
#include "parse.hpp"

class Table {
    Ast *ast;
    Context ctx;
    std::string *vars;

    void solve(unsigned int numvars, unsigned int rows, unsigned int cols);

  public:
    Table(Ast *ast);
    ~Table();
    unsigned int numvars, rows, cols;
    std::string *truth_table;

    std::string *get_vars() const;

    friend std::ostream &operator<<(std::ostream &os, const Table &table);
};

#endif
