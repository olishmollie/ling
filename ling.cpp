#include "ast.hpp"
#include "parse.hpp"
#include "solver.hpp"
#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <set>

using namespace std;

void print_vec(std::vector<Implicant> &impls);
void print_vec(std::vector<std::vector<bool>> &table);

int main() {
    string input;

    cout << "Y>> ";
    while (getline(cin, input)) {
        unsigned int start = 0;

        try {
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);

            // the empty string parses to nullptr
            if (!ast) {
                cout << "Y>> ";
                continue;
            }

            Table table(ast);
            Solver solver(&table);
            cout << solver.solve() << endl;

        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }

        cout << "Y>> ";

    }
    cout << endl;
}

void print_vec(std::vector<Implicant> &impls) {
    for (auto impl : impls) {
        cout << impl.to_string() << endl;
    }
    cout << endl;
}

void print_vec(std::vector<std::vector<bool>> &table) {
    for (unsigned int i = 0; i < table.size(); i++) {
        cout << i << ":";
        for (unsigned int j = 0; j < table[i].size(); j++) {
            cout << (table[i][j] ? "X" : " ");
        }
        cout << endl;
    }
    cout << endl;
}
