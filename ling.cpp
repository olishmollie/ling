#include "ast.hpp"
#include "parse.hpp"

using namespace std;
using namespace ling;

int main() {
    init();
    string input;
    cout << ">> ";
    while (getline(cin, input)) {
        unsigned int start = 0;
        try {
            Evaluator ev;
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);
            Ast *evaluated = ast->accept(ev);
            cout << evaluated->to_string() << endl;
            delete ast;
            delete evaluated;
        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }
        cout << ">> ";
    }
    cout << endl;
    cleanup();
}
