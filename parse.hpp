#ifndef _PARSE_H
#define _PARSE_H

#include "ast.hpp"

#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

namespace ling {

using std::exception;
using std::string;
using std::stringstream;
using std::vector;

//////////////////// Lexing ///////////////////////
struct ScanError : exception {
    string msg;
    ScanError(string msg) : msg(msg) {}
    ~ScanError() throw() {}
};

bool is_delim(char c) {
    return c == '-' || c == '\'' || c == '(' || c == ')' || c == '+' ||
           c == '*';
}

vector<Token> tokenize(std::string input) {
    vector<Token> tokens;
    stringstream ss(input);
    while (!ss.eof()) {
        Token token;
        char c = ss.get();
        if (c == EOF) {
            break;
        } else if (is_delim(c)) {
            token = string(1, c);
            if (c == '-') {
                if (ss.peek() != '>') {
                    goto error;
                }
                token.append(1, ss.get());
            }
            tokens.push_back(token);
        } else if (isalpha(c)) {
            while (isalpha(c)) {
                token.append(1, c);
                c = ss.get();
            }
            ss.unget();
            tokens.push_back(token);
        } else if (isspace(c)) {
            continue; // skip spaces
        } else {
        error:
            throw ScanError("invalid character '" + string(1, c) + "'");
        }
    }
    return tokens;
}

void print_tokens(vector<Token> &tokens) {
    std::cout << "[";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i];
        if (i != tokens.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

/////////////////// Parsing /////////////////////////
struct ParseError : exception {
    string msg;
    ParseError(string msg) : msg(msg) {}
    ~ParseError() throw() {}
};

Token next(vector<Token> &tokens, unsigned int &pos) {
    if (pos >= tokens.size()) {
        return "";
    }
    return tokens[pos++];
}

Token peek(vector<Token> &tokens, unsigned int pos) {
    if (pos >= tokens.size()) {
        return "";
    }
    return tokens[pos];
}

void expect(Token expected, vector<Token> &tokens, unsigned int &pos) {
    Token actual = next(tokens, pos);
    if (actual != expected) {
        throw ParseError("expected token " + expected);
    }
}

Ast *parse(vector<Token> &, unsigned int &);

Ast *identifier(vector<Token> &tokens, unsigned int &pos) {
    Token token = next(tokens, pos);
    if (token == "") {
        return EmptyString;
    } else if (token == "(") {
        Ast *group_ast = parse(tokens, pos);
        expect(")", tokens, pos);
        return group_ast;
    } else {
        if ((token < "a" || token > "z") && (token < "A" || token > "Z")) {
            throw ParseError("invalid token " + token);
        }
        if (token == "true") {
            return T;
        } else if (token == "false") {
            return F;
        }
        return new Identifier(token);
    }
}

Ast *negation(vector<Token> &tokens, unsigned int &pos) {
    Ast *child = identifier(tokens, pos);
    Token op = peek(tokens, pos);
    // the grammar says this should be recursive?
    while (op == "'") {
        next(tokens, pos);
        child = new Unop(op, child);
        op = peek(tokens, pos);
    }
    return child;
}

Ast *conjunction(vector<Token> &tokens, unsigned int &pos) {
    Ast *lhs = negation(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "*") {
        next(tokens, pos);
        return new Binop(op, lhs, conjunction(tokens, pos));
    }
    return lhs;
}

Ast *disjunction(vector<Token> &tokens, unsigned int &pos) {
    Ast *lhs = conjunction(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "+") {
        next(tokens, pos);
        return new Binop(op, lhs, disjunction(tokens, pos));
    }
    return lhs;
}

Ast *implication(vector<Token> &tokens, unsigned int &pos) {
    Ast *lhs = disjunction(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "->") {
        next(tokens, pos);
        return new Binop(op, lhs, implication(tokens, pos));
    }
    return lhs;
}

Ast *parse(vector<Token> &tokens, unsigned int &pos) {
    return implication(tokens, pos);
}

void init() {
    T = new Identifier("true");
    F = new Identifier("false");
    EmptyString = new Identifier("");
}

void cleanup() {
    delete T;
    delete F;
    delete EmptyString;
}

} // namespace ling

#endif
