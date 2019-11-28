#ifndef _AST_H
#define _AST_H

#include <iostream>
#include <string>

namespace ling {

using std::string;

typedef string Token;

struct Ast;
struct Evaluator;

Ast *T;
Ast *F;
Ast *EmptyString;

struct Ast {
    virtual string to_string() const = 0;
    virtual Ast *accept(Evaluator &ev) = 0;
    virtual Ast *complement();
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
    string to_string() const { return ident; }
    Ast *accept(Evaluator &ev);
};

struct Unop : public Ast {
    const Token op;
    Ast *child;
    Unop() {}
    Unop(Token op, Ast *child) : op(op), child(child) {}
    string to_string() const { return child->to_string() + "'"; }
    Ast *accept(Evaluator &ev);
    ~Unop() { delete child; }
};

struct Binop : public Ast {
    const Token op;
    Ast *lhs;
    Ast *rhs;
    Binop() {}
    Binop(Token op, Ast *lhs, Ast *rhs) : op(op), lhs(lhs), rhs(rhs) {}
    string to_string() const {
        return "(" + lhs->to_string() + " " + op + " " + rhs->to_string() + ")";
    }
    Ast *accept(Evaluator &ev);
    Ast *deMorgan();
    ~Binop() {
        delete lhs;
        delete rhs;
    }
};

// HACK: I'm not sure how to do this elegantly. Have to compare
// strings for now, not to mention that operator== doesn't
// want to take pointers to an abstract class, so to use this
// one has to dereference: *a == *b. Blargh.
bool operator==(Ast &lhs, Ast &rhs) {
    return lhs.to_string() == rhs.to_string();
}
bool operator==(Ast &lhs, const char *rhs) {
    return lhs.to_string() == rhs;
}
// end HACK

Ast *Ast::complement() {
    return new Unop("'", this);
}

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

    // HACK: This is needed to resolve involution.
    // There's got to be a better way.
    Unop *unop = dynamic_cast<Unop *>(child);
    if (unop) {
        return unop->child->accept(*this);
    }

    if (child == T || child == F) {
        return child == T ? F : T;
    } else if (un->child == un) {
        return un->child;
    }

    return new Unop(un->op, child);
}

Ast *Evaluator::eval(Binop *bn) {
    Ast *lhs = bn->lhs->accept(*this);
    Ast *rhs = bn->rhs->accept(*this);
    if (bn->op == "+") {
        if (*lhs == *rhs) { // Idempotence
            return lhs;
        } else if (*lhs == *rhs->complement() ||
                   *rhs == *lhs->complement()) { // Complementarity
            return T;
        } else if (lhs == T || rhs == T) { // Dominance
            return T;
        } else if (lhs == F || rhs == F) { // Identity
            return lhs == F ? rhs : lhs;
        }
    } else if (bn->op == "*") {
        if (*lhs == *rhs) { // Idempotence
            return lhs;
        } else if (*lhs == *rhs->complement() ||
                   *rhs == *lhs->complement()) { // Complementarity
            return F;
        } else if (lhs == F || rhs == F) { // Dominance
            return F;
        } else if (lhs == T || rhs == T) { // Identity
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

Ast *Binop::deMorgan() {
    return NULL;
}

} // namespace ling

#endif
