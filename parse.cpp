#include "parse.hpp"

#include <exception>
#include <iostream>
#include <sstream>

//////////////////// Lexing ///////////////////////
bool is_delim(char c) {
    return c == '-' || c == '\'' || c == '(' || c == ')' || c == '+' ||
           c == '*' || c == '=';
}

TokenVec tokenize(std::string input) {
    TokenVec tokens;
    std::stringstream ss(input);
    while (!ss.eof()) {
        Token token;
        char c = ss.get();
        if (c == EOF) {
            break;
        } else if (is_delim(c)) {
            token = std::string(1, c);
            if (c == '-') {
                if (ss.peek() != '>') {
                    throw ScanError("invalid character '" +
                                    std::string(1, ss.peek()) + "'");
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
            throw ScanError("invalid character '" + std::string(1, c) + "'");
        }
    }
    return tokens;
}

void print_tokens(TokenVec &tokens) {
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
Token next(TokenVec &tokens, unsigned int &pos) {
    if (pos >= tokens.size()) {
        return "";
    }
    return tokens[pos++];
}

Token peek(TokenVec &tokens, unsigned int pos) {
    if (pos >= tokens.size()) {
        return "";
    }
    return tokens[pos];
}

void expect(Token expected, TokenVec &tokens, unsigned int &pos) {
    Token actual = next(tokens, pos);
    if (actual != expected) {
        throw ParseError("expected token " + expected);
    }
}

Ast *parse(TokenVec &, unsigned int &);

Ast *identifier(TokenVec &tokens, unsigned int &pos) {
    Token token = next(tokens, pos);
    if (token == "") {
        throw ParseError("unexpected eof");
    } else if (token == "(") {
        Ast *group_ast = parse(tokens, pos);
        expect(")", tokens, pos);
        group_ast->grouped = true;
        return group_ast;
    } else {
        if ((token < "a" || token > "z") && (token < "A" || token > "Z")) {
            throw ParseError("invalid token " + token);
        }
        return new Identifier(token);
    }
}

Ast *negation(TokenVec &tokens, unsigned int &pos) {
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

Ast *conjunction(TokenVec &tokens, unsigned int &pos) {
    Ast *lhs = negation(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "*") {
        next(tokens, pos);
        return new Binop(op, lhs, conjunction(tokens, pos));
    }
    return lhs;
}

Ast *disjunction(TokenVec &tokens, unsigned int &pos) {
    Ast *lhs = conjunction(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "+") {
        next(tokens, pos);
        return new Binop(op, lhs, disjunction(tokens, pos));
    }
    return lhs;
}

Ast *implication(TokenVec &tokens, unsigned int &pos) {
    Ast *lhs = disjunction(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "->") {
        next(tokens, pos);
        return new Binop(op, lhs, implication(tokens, pos));
    }
    return lhs;
}

Ast *equivalence(TokenVec &tokens, unsigned int &pos) {
    Ast *lhs = implication(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "=") {
        next(tokens, pos);
        return new Binop(op, lhs, equivalence(tokens, pos));
    }
    return lhs;
}

Ast *parse(TokenVec &tokens, unsigned int &pos) {
    if (tokens.empty()) {
        return nullptr;
    }
    return equivalence(tokens, pos);
}
