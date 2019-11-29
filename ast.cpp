#include "ast.hpp"

std::string Identifier::to_string() const {
    return ident;
}

bool Identifier::accept(Evaluator &ev, Context &ctx) {
    return ev.eval(this, ctx);
}

void Identifier::add_to_context(Context &ctx) const {
    ctx.insert(std::pair<Token, bool>(ident, false));
}

std::string Unop::to_string() const {
    return child->to_string() + "'";
}

bool Unop::accept(Evaluator &ev, Context &ctx) {
    return ev.eval(this, ctx);
}

void Unop::add_to_context(Context &ctx) const {
    child->add_to_context(ctx);
}

std::string Binop::to_string() const {
    return "(" + lhs->to_string() + " " + op + " " + rhs->to_string() + ")";
}

bool Binop::accept(Evaluator &ev, Context &ctx) {
    return ev.eval(this, ctx);
}

void Binop::add_to_context(Context &ctx) const {
    lhs->add_to_context(ctx);
    rhs->add_to_context(ctx);
}

bool Evaluator::eval(Identifier *id, Context &ctx) {
    Context::iterator val = ctx.find(id->ident);
    if (val != ctx.end()) {
        return val->second;
    }
    throw EvalError("unbound variable " + id->ident);
}

bool Evaluator::eval(Unop *un, Context &ctx) {
    bool val = un->child->accept(*this, ctx);
    return !val;
}

bool Evaluator::eval(Binop *bn, Context &ctx) {
    bool lhs = bn->lhs->accept(*this, ctx);
    bool rhs = bn->rhs->accept(*this, ctx);
    if (bn->op == "+") {
        return lhs || rhs;
    } else if (bn->op == "*") {
        return lhs && rhs;
    } else { // binop == "->"
        return !lhs || rhs;
    }
}
