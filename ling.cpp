#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ling {

using std::exception;
using std::string;
using std::stringstream;
using std::vector;

#ifdef DEBUG
using std::cout;
using std::endl;
#endif

/////////////////// Types /////////////////////////
typedef string Token;

struct Ast {
#ifdef DEBUG
    virtual void print(int depth = 0) const = 0;
#endif
    virtual string eval() const = 0;
    virtual ~Ast() {}
};

struct Identifier : public Ast {
    const Token ident;
    Identifier(){};
    Identifier(Token ident) : ident(ident) {}
#ifdef DEBUG
    void print(int depth) const {
        string spaces = string(2 * depth, ' ');
        cout << spaces << "ident: " << ident << endl;
    }
#endif
    string eval() const { return ident; }
};

struct Unop : public Ast {
    const Token op;
    const Ast *child;
    Unop() {}
    Unop(Token op, Ast *child) : op(op), child(child) {}
    ~Unop() { delete child; }
#ifdef DEBUG
    void print(int depth) const {
        string spaces = string(2 * depth, ' ');
        cout << spaces << "unop: NOT" << endl;
        child->print(depth + 2);
    }
#endif
    string eval() const { return child->eval() + "'"; }
};

struct Binop : public Ast {
    const Token op;
    const Ast *lhs;
    const Ast *rhs;
    Binop() {}
    Binop(Token op, Ast *lhs, Ast *rhs) : op(op), lhs(lhs), rhs(rhs) {}
    ~Binop() {
        delete lhs;
        delete rhs;
    }
#ifdef DEBUG
    void print(int depth) const {
        string spaces = string(2 * depth, ' ');
        cout << spaces << "binop: " << op << endl;
        lhs->print(depth + 2);
        rhs->print(depth + 2);
    }
#endif
    string eval() const {
        return "(" + lhs->eval() + " " + op + " " + rhs->eval() + ")";
    }
};

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
        char c = ss.get();
        if (c == EOF) {
            break;
        } else if (is_delim(c)) {
            Token token = string(1, c);
            if (c == '-') {
                if (ss.peek() != '>') {
                    goto error;
                }
                token.append(1, ss.get());
            }
            tokens.push_back(token);
        } else if (isalpha(c)) {
            Token token;
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

#ifdef DEBUG
void print_tokens(vector<Token> &tokens) {
    cout << "[";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        cout << tokens[i];
        if (i != tokens.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}
#endif

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
        return new Identifier();
    } else if (token == "(") {
        Ast *group_ast = parse(tokens, pos);
        expect(")", tokens, pos);
        return group_ast;
    } else {
        if (token < "a" || token > "z") {
            throw ParseError("invalid token " + token);
        }
        return new Identifier(token);
    }
}

Ast *negation(vector<Token> &tokens, unsigned int &pos) {
    Ast *child = identifier(tokens, pos);
    Token op = peek(tokens, pos);
    if (op == "'") {
        next(tokens, pos);
        return new Unop(op, child);
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

} // namespace ling

///////////////////// Main ///////////////////////

using namespace std;
using namespace ling;

int main() {
    string input;
    cout << ">> ";
    while (getline(cin, input)) {
        unsigned int start = 0;
        try {
            vector<Token> tokens = tokenize(input);
            Ast *ast = parse(tokens, start);
            cout << ast->eval() << endl;
#ifdef DEBUG
            print_tokens(tokens);
            ast->print();
#endif
            delete ast;
        } catch (const ScanError &e) {
            cerr << "scan error: " << e.msg << endl;
        } catch (const ParseError &e) {
            cerr << "parse error: " << e.msg << endl;
        }
        cout << ">> ";
    }
    cout << endl;
}
