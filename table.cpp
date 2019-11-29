#include "table.hpp"

#include <iomanip>
#include <map>
#include <sstream>

Table::Table(Ast *ast) : ast(ast) {
    unsigned int numvars, rows, cols;
    ast->add_to_context(ctx);
    numvars = ctx.size();
    rows = 2 << (numvars - 1);
    cols = numvars + 1;
    init(numvars, rows, cols);
    solve();
}

Table::~Table() {
    for (unsigned int i = 0; i < rows; i++) {
        delete[] truth_table[i];
    }
    delete[] truth_table;
    delete ast;
}

void Table::init(unsigned int numvars, unsigned int rows, unsigned int cols) {
    truth_table = new bool *[rows];
    for (unsigned int i = 0; i < rows; i++) {
        truth_table[i] = new bool[cols];
    }
    this->numvars = numvars;
    this->rows = rows;
    this->cols = cols;
}

void Table::solve() {
    Evaluator ev;
    truth_table[0][numvars] = ast->accept(ev, ctx);
    for (unsigned int i = 0; i < rows; i++) {
        unsigned j = 0;
        for (auto &pair : ctx) {
            if (pair.first == "t") {
                pair.second = true;
            } else if (pair.first == "f") {
                pair.second = false;
            } else {
                pair.second = i >> ((numvars - 1 - j)) & 1;
            }
            truth_table[i][j++] = pair.second;
        }
        truth_table[i][numvars] = ast->accept(ev, ctx);
    }
}

// oof.
std::ostream &operator<<(std::ostream &ss, const Table &table) {
    std::string ast_str = table.ast->to_string();
    std::vector<int> lengths;
    ss << " ";
    unsigned int ctx_size = 0;
    for (auto &pair : table.ctx) {
        lengths.push_back(pair.first.length());
        ctx_size += pair.first.length() + 1;
        ss << pair.first << " ";
    }
    ss << "| " << ast_str << std::endl;
    unsigned int size = ast_str.length() + ctx_size + 2;
    for (unsigned int i = 0; i < size + 1; i++) {
        ss << (i == ctx_size + 1 ? "|" : "-");
    }
    ss << std::endl;
    for (unsigned int i = 0; i < table.rows; i++) {
        ss << " ";
        for (unsigned int j = 0; j < table.cols; j++) {
            if (j == table.cols - 1) {
                ss << std::setw(size - ctx_size - 2);
            } else {
                ss << std::setw(lengths[j]);
            }
            ss << std::right << table.truth_table[i][j];
            if (j != table.cols - 1) {
                ss << " ";
            }
            if (j == table.cols - 2) {
                ss << "| ";
            }
        }
        if (i != table.rows - 1) {
            ss << std::endl;
        }
    }
    return ss;
}
