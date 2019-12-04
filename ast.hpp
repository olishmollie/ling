#ifndef _AST_H
#define _AST_H

#include "typedef.hpp"

#include <iostream>
#include <map>
#include <string>

struct Ast;
struct Evaluator;

struct Ast {
    bool grouped = false;
    std::string to_string() const;
    virtual std::string to_string_virtual() const = 0;
    virtual bool accept(Evaluator &ev, Context &ctx) = 0;
    virtual void add_to_context(Context &ctx) const = 0;
    virtual ~Ast() {}
};

std::ostream &operator<<(std::ostream &os, const Ast *ast);

struct Identifier : public Ast {
    const Token ident;
    Identifier(){};
    Identifier(Token ident) : ident(ident) {}
    std::string to_string_virtual() const;
    bool accept(Evaluator &ev, Context &ctx);
    void add_to_context(Context &ctx) const;
};

struct Unop : public Ast {
    const Token op;
    Ast *child;
    Unop() {}
    Unop(Token op, Ast *child) : op(op), child(child) {}
    std::string to_string_virtual() const;
    bool accept(Evaluator &ev, Context &ctx);
    void add_to_context(Context &ctx) const;
    ~Unop() { delete child; }
};

struct Binop : public Ast {
    const Token op;
    Ast *lhs;
    Ast *rhs;
    Binop() {}
    Binop(Token op, Ast *lhs, Ast *rhs) : op(op), lhs(lhs), rhs(rhs) {}
    std::string to_string_virtual() const;
    bool accept(Evaluator &ev, Context &ctx);
    void add_to_context(Context &ctx) const;
    ~Binop() {
        delete lhs;
        delete rhs;
    }
};

struct EvalError : std::exception {
    std::string msg;
    EvalError(std::string msg) : msg(msg) {}
    ~EvalError() throw() {}
};

struct Evaluator {
    bool eval(Identifier *id, Context &ctx);
    bool eval(Unop *un, Context &ctx);
    bool eval(Binop *bn, Context &ctx);
};

#endif
