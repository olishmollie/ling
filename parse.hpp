#ifndef _PARSE_H
#define _PARSE_H

#include "ast.hpp"

#include <vector>

typedef std::vector<Token> TokenVec;

struct ScanError : std::exception {
    std::string msg;
    ScanError(std::string msg) : msg(msg) {}
    ~ScanError() throw() {}
};

struct ParseError : std::exception {
    std::string msg;
    ParseError(std::string msg) : msg(msg) {}
    ~ParseError() throw() {}
};

TokenVec tokenize(std::string input);
Ast *parse(TokenVec &tokens, unsigned int &pos);
void print_tokens(TokenVec &tokens);

#endif
