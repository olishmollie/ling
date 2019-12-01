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

    cout << ">> ";
    while (getline(cin, input)) {
        unsigned int start = 0;

        try {
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);

            // the empty string parses to nullptr
            if (!ast) {
                continue;
            }

            Table table(ast);
            cout << table << endl << endl;

            Solver solver(&table);
            std::set<Implicant> essential_primes = solver.solve();

            if (essential_primes.empty()) {
                std::cout << "F" << std::endl;

            } else if (essential_primes.size() == 1 &&
                       essential_primes.begin()->is_true()) {
                std::cout << "T" << std::endl;
            } else {
                solver.print_impls();
            }

        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }
        cout << ">> ";
    }
    cout << endl;
}
