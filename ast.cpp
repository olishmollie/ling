#include "ast.hpp"

#include <sstream>

std::string Ast::to_string() const {
    std::stringstream ss;
    if (grouped) {
        ss << "(";
    }
    ss << to_string_virtual();
    if (grouped) {
        ss << ")";
    }
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Ast *ast) {
    os << ast->to_string();
    return os;
}

std::string Identifier::to_string_virtual() const {
    return ident;
}

bool Identifier::accept(Evaluator &ev, Context &ctx) {
    return ev.eval(this, ctx);
}

void Identifier::add_to_context(Context &ctx) const {
    ctx.insert(std::pair<Token, bool>(ident, false));
}

std::string Unop::to_string_virtual() const {
    std::stringstream ss;
    ss << child->to_string() << "'";
    return ss.str();
}

bool Unop::accept(Evaluator &ev, Context &ctx) {
    return ev.eval(this, ctx);
}

void Unop::add_to_context(Context &ctx) const {
    child->add_to_context(ctx);
}

std::string Binop::to_string_virtual() const {
    std::stringstream ss;
    ss << lhs->to_string() << " " << op << " " << rhs->to_string();
    return ss.str();
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
    } else if (bn->op == "=") {
        return lhs == rhs;
    } else { // binop == "->"
        return !lhs || rhs;
    }
}
