#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <map>
#include <string>

using std::cout;
using std::endl;
using std::ostream;
using std::right;
using std::setw;
using std::string;

Table new_table(Ast *ast) {
    Table table;
    table.ast = ast;

    ast->add_to_context(table.ctx);
    for (auto &pair : table.ctx) {
        table.vars.push_back(pair.first);
    }

    table.rows = 2 << (table.vars.size() - 1);
    table.cols = table.vars.size() + 1;
    table.data = std::vector<string>(table.rows, string(table.cols, '-'));

    return table;
}

char get_output(const Table &self, const unsigned int row) {
    return self.data[row][self.vars.size()];
}

void set_output(Table &self, unsigned int row, char val) {
    self.data[row][self.vars.size()] = val;
}

void solve(Table &self) {
    // The evaluator visits each node in the AST.
    Evaluator ev;

    for (unsigned int i = 0; i < self.rows; i++) {
        // This loop updates the context. The map is ordered, so this is the way
        // I came up with to increment the bools in the map as if they were
        // binary strings. Could probably do with a refactor.
        for (unsigned int j = 0; j < self.vars.size(); j++) {
            string var = self.vars[j];
            bool &val = self.ctx[var];

            // If there's a T or F, they map to their boolean values
            if (var == "T") {
                val = true;
            } else if (var == "T") {
                val = false;
            }

            // This one is interesting. The most significant 'bit' (really a
            // key value pair) in the ordered map corresponds to the row
            // shifted by a certain number of bits. Then we and by one to
            // get the boolean.
            else {
                val = i >> ((self.vars.size() - 1 - j)) & 1;
            }

            // Assign the boolean to this variable in the context.
            self.data[i][j] = val + '0';
        }

        // Evaluate the expression for the given context, and assign
        // it to the output column.
        char output = self.ast->accept(ev, self.ctx) + '0';
        set_output(self, i, output);
    }
}

// Oof.
void print_table(Table &table) {
    string ast_str;
    unsigned int len_inputs, row_len;

    // print heading
    cout << " ";
    len_inputs = 0;
    for (unsigned int i = 0; i < table.vars.size(); i++) {
        len_inputs += table.vars[i].length() + 1;
        cout << table.vars[i] << " ";
    }
    ast_str = table.ast->to_string();
    cout << "| " << ast_str << endl;

    // print "----" below heading
    row_len = ast_str.length() + len_inputs + 2;
    for (unsigned int i = 0; i < row_len + 1; i++) {
        cout << (i == len_inputs + 1 ? '|' : '-');
    }
    cout << endl;

    // print table
    for (unsigned int i = 0; i < table.rows; i++) {
        cout << " ";
        for (unsigned int j = 0; j < table.cols; j++) {
            if (j == table.cols - 1) {
                cout << setw(row_len - len_inputs - 2);
            } else {
                cout << setw(table.vars[j].length());
            }
            cout << right << table.data[i][j];
            if (j != table.cols - 1) {
                cout << " ";
            }
            if (j == table.cols - 2) {
                cout << "| ";
            }
        }
        if (i != table.rows - 1) {
            cout << endl;
        }
    }
}
