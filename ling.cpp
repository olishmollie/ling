#include "ast.hpp"
#include "parse.hpp"
#include "solver.hpp"
#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <set>

using namespace std;

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

            Table table = new_table(ast);
            solve(table);

            Solver solver = new_solver(&table);
            Ast *output = solve(solver);
            cout << output << endl;

        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }

        cout << "Y>> ";
    }
    cout << endl;
}
