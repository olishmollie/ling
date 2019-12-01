#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <map>

Table::Table(Ast *ast) : ast(ast) {
    unsigned int numvars, rows, cols;

    // A context is just a hash table that maps variables to their
    // boolean values at any given time.
    ast->add_to_context(ctx);

    // The number of boolean variables present in the expression.
    numvars = ctx.size();

    // The number of rows in the truth table equals 2^(n-1)
    rows = 2 << (numvars - 1);

    // The extra column is for the output
    cols = numvars + 1;

    solve(numvars, rows, cols);

    // We do this afterward to make sure its exception safe.
    this->numvars = numvars;
    this->rows = rows;
    this->cols = cols;
}

Table::~Table() {
    delete[] truth_table;
    delete ast;
}

void Table::solve(unsigned int numvars, unsigned int rows, unsigned int cols) {
    // The evaluator visits each node in the AST.
    Evaluator ev;

    // We keep an array of the variables for printing them nicely later.
    vars = new std::string[numvars];

    // The actual truth table is an array of strings, where each character is
    // a column in the table.
    truth_table = new std::string[rows];

    for (unsigned int i = 0; i < rows; i++) {
        // Create a new row.
        truth_table[i] = std::string(cols, ' ');

        // This loop updates the context. The map is ordered, so this is the way
        // I came up with to increment the bools in the map as if they were
        // binary strings. Could probably do with a refactor.
        unsigned j = 0;
        for (auto &pair : ctx) {

            // Populate the vars array. Only necessary the first go around.
            if (i == 0) {
                vars[j] = pair.first;
            }

            // If there's a T or F, they map to their boolean values
            if (pair.first == "T") {
                pair.second = true;
            } else if (pair.first == "T") {
                pair.second = false;
            }

            // This one is interesting. The most significant 'bit' (really a
            // key value pair) in the ordered map corresponds to the row
            // shifted by a certain number of bits. Then we and by one to
            // get the boolean.
            else {
                pair.second = i >> ((numvars - 1 - j)) & 1;
            }

            // Assign the boolean to this variable in the context.
            truth_table[i][j++] = pair.second + '0';
        }

        // Evaluate the expression for the given context, and assign
        // it to the output column.
        truth_table[i][numvars] = ast->accept(ev, ctx) + '0';
    }
}

// Oof.
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
