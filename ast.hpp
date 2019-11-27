#ifndef _AST_H
#define _AST_H

#include <iostream>
#include <string>

namespace ling {

typedef std::string Token;

struct Ast;
struct Evaluator;

Ast *T;
Ast *F;
Ast *EmptyString;

struct Ast {
    virtual std::string to_string() const = 0;
    virtual Ast *accept(Evaluator &ev) = 0;
    virtual ~Ast() {}
    void operator delete(void *ast) {
        if (ast != T && ast != F && ast != EmptyString) {
            free(ast);
        }
    }
};

struct Identifier : public Ast {
    const Token ident;
    Identifier(){};
    Identifier(Token ident) : ident(ident) {}
    std::string to_string() const { return ident; }
    Ast *accept(Evaluator &ev);
};

struct Unop : public Ast {
    const Token op;
    Ast *child;
    Unop() {}
    Unop(Token op, Ast *child) : op(op), child(child) {}
    std::string to_string() const { return child->to_string() + "'"; }
    Ast *accept(Evaluator &ev);
    ~Unop() { delete child; }
};

struct Binop : public Ast {
    const Token op;
    Ast *lhs;
    Ast *rhs;
    Binop() {}
    Binop(Token op, Ast *lhs, Ast *rhs) : op(op), lhs(lhs), rhs(rhs) {}
    std::string to_string() const {
        return "(" + lhs->to_string() + " " + op + " " + rhs->to_string() + ")";
    }
    Ast *accept(Evaluator &ev);
    ~Binop() {
        delete lhs;
        delete rhs;
    }
};

struct Evaluator {
    Ast *eval(Identifier *id);
    Ast *eval(Unop *un);
    Ast *eval(Binop *bn);
};

Ast *Identifier::accept(Evaluator &ev) {
    return ev.eval(this);
}

Ast *Unop::accept(Evaluator &ev) {
    return ev.eval(this);
}

Ast *Binop::accept(Evaluator &ev) {
    return ev.eval(this);
}

Ast *Evaluator::eval(Identifier *id) {
    if (id == T || id == F) {
        return id;
    }
    return new Identifier(id->ident);
}

Ast *Evaluator::eval(Unop *un) {
    Ast *child = un->child->accept(*this);
    if (child == T || child == F) {
        return child == T ? F : T;
    }
    return new Unop(un->op, child);
}

Ast *Evaluator::eval(Binop *bn) {
    Ast *lhs = bn->lhs->accept(*this);
    Ast *rhs = bn->rhs->accept(*this);
    if (bn->op == "+") {
        if (lhs == T || rhs == T) {
            return T;
        } else if (lhs == F && rhs == F) {
            return F;
        } else if (lhs == F || rhs == F) {
            return lhs == F ? rhs : lhs;
        }
    } else if (bn->op == "*") {
        if (lhs == F || rhs == F) {
            return F;
        } else if (lhs == T && rhs == T) {
            return T;
        } else if (lhs == T || rhs == T) {
            return lhs == T ? rhs : lhs;
        }
    } else if (bn->op == "->") {
        if (lhs == F || (lhs == T && rhs == T)) {
            return T;
        } else if (lhs == T && rhs == F) {
            return F;
        }
    }
    return new Binop(bn->op, lhs, rhs);
}

} // namespace ling
#endif
