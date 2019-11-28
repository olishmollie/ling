#include "../ast.hpp"
#include "../parse.hpp"

#include <assert.h>
#include <string>
#include <vector>

using namespace ling;
using std::string;
using std::vector;

Evaluator ev;

Ast *evaluate(string input) {
    unsigned int start = 0;
    vector<Token> tokens = tokenize(input);
    Ast *ast = parse(tokens, start);
    Ast *evaluated = ast->accept(ev);
    delete ast;
    return evaluated;
}

void test_logical_inverse() {
    Ast *evaluated;

    evaluated = evaluate("true'");
    assert(evaluated == F);
    delete evaluated;

    evaluated = evaluate("false'");
    assert(evaluated == T);
    delete evaluated;
}

void test_involution() {
    Ast *evaluated;

    evaluated = evaluate("a''");
    assert(*evaluated == "a");
    delete evaluated;
}

void test_dominance() {
    Ast *evaluated;

    evaluated = evaluate("a + true");
    assert(evaluated == T);
    delete evaluated;

    evaluated = evaluate("a * false");
    assert(evaluated == F);
    delete evaluated;
}

void test_identity() {
    Ast *evaluated;

    evaluated = evaluate("a + false");
    assert(evaluated->to_string() == "a");
    delete evaluated;

    evaluated = evaluate("a * true");
    assert(evaluated->to_string() == "a");
    delete evaluated;
}

void test_idempotence() {
    Ast *evaluated;

    evaluated = evaluate("a + a");
    assert(evaluated->to_string() == "a");
    delete evaluated;

    evaluated = evaluate("a * a");
    assert(evaluated->to_string() == "a");
    delete evaluated;
}

void test_complementarity() {
    Ast *evaluated;

    evaluated = evaluate("a + a'");
    assert(evaluated == T);
    delete evaluated;

    evaluated = evaluate("a * a'");
    assert(evaluated == F);
    delete evaluated;
}

int main() {
    init();

    test_logical_inverse();
    test_involution();
    test_dominance();
    test_idempotence();
    test_complementarity();

    cleanup();
}
