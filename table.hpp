#ifndef _TABLE_H
#define _TABLE_H

#include "parse.hpp"

#include <vector>

class Table {
    Ast *ast;
    Context ctx;
    unsigned int numvars, rows, cols;
    std::string *vars;
    int **truth_table;

    void init(unsigned int numvars, unsigned int rows, unsigned int cols);

  public:
    Table(Ast *ast);
    ~Table();
    friend std::ostream &operator<<(std::ostream &os, const Table &table);
};

#endif
