#include "ast.hpp"
#include "parse.hpp"
#include "solver.hpp"
#include "table.hpp"

#include <iomanip>
#include <iostream>
#include <set>
#include <string>

#include <readline/history.h>
#include <readline/readline.h>

using namespace std;

int main() {
    char *input;

    while ((input = readline("Y>> ")) != nullptr) {
        unsigned int start = 0;

        try {
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);

            // An empty string parses to nullptr.
            if (!ast) {
                continue;
            }

            Table table = new_table(ast);
            solve(table);

            Solver solver = new_solver(&table);
            Ast *output = solve(solver);

            cout << output << endl;
            delete output;

            // Add any actual input to history.
            if (strlen(input) > 0) {
                add_history(input);
            }

            free(input);

        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }
    }
    cout << endl;
}
