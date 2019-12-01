#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <map>

Table::Table(Ast *ast) : ast(ast) {
    unsigned int numvars, rows, cols;
    ast->add_to_context(ctx);
    numvars = ctx.size();
    rows = 2 << (numvars - 1);
    cols = numvars + 1;
    solve(numvars, rows, cols);
    this->numvars = numvars;
    this->rows = rows;
    this->cols = cols;
}

Table::~Table() {
    delete[] truth_table;
    delete ast;
}

void Table::solve(unsigned int numvars, unsigned int rows, unsigned int cols) {
    Evaluator ev;
    vars = new std::string[numvars];
    truth_table = new std::string[rows];
    for (unsigned int i = 0; i < rows; i++) {
        truth_table[i] = std::string(cols, ' ');
        unsigned j = 0;
        for (auto &pair : ctx) {
            if (i == 0) {
                vars[j] = pair.first;
            }
            if (pair.first == "t") {
                pair.second = true;
            } else if (pair.first == "f") {
                pair.second = false;
            } else {
                pair.second = i >> ((numvars - 1 - j)) & 1;
            }
            truth_table[i][j++] = pair.second + '0';
        }
        truth_table[i][numvars] = ast->accept(ev, ctx) + '0';
    }
}

// oof.
std::ostream &operator<<(std::ostream &os, const Table &table) {
    std::string ast_str;
    unsigned int len_inputs, row_len;

    // print heading
    os << " ";
    len_inputs = 0;
    for (unsigned int i = 0; i < table.numvars; i++) {
        len_inputs += table.vars[i].length() + 1;
        os << table.vars[i] << " ";
    }
    ast_str = table.ast->to_string();
    os << "| " << ast_str << std::endl;

    // print "----" below heading
    row_len = ast_str.length() + len_inputs + 2;
    for (unsigned int i = 0; i < row_len + 1; i++) {
        os << (i == len_inputs + 1 ? '|' : '-');
    }
    os << std::endl;

    // print table
    for (unsigned int i = 0; i < table.rows; i++) {
        os << " ";
        for (unsigned int j = 0; j < table.cols; j++) {
            if (j == table.cols - 1) {
                os << std::setw(row_len - len_inputs - 2);
            } else {
                os << std::setw(table.vars[j].length());
            }
            os << std::right << table.truth_table[i][j];
            if (j != table.cols - 1) {
                os << " ";
            }
            if (j == table.cols - 2) {
                os << "| ";
            }
        }
        if (i != table.rows - 1) {
            os << std::endl;
        }
    }

    return os;
}
