#include "ast.hpp"
#include "parse.hpp"
#include "table.hpp"
#include "solver.hpp"

#include <iomanip>
#include <iostream>

using namespace std;

int main() {
    string input;
    cout << ">> ";
    while (getline(cin, input)) {
        unsigned int start = 0;
        try {
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);
            if (ast) {
                Table table(ast);
                cout << table << endl << endl;
                Solver solver(&table);
                solver.run();
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
