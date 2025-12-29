#ifndef expression_hpp
#define expression_hpp

#include "variable.hpp"
#include "game_state.hpp"
#include "tokenizer.hpp"

enum class ExprType{
    BinaryExpr,
    UnaryExpr,
    Assignment,
    Var
};

// (expr) = expr

class Expression;

struct BinaryExpression{
    Expression* l;
    Operand oper;
    Expression* r;
};

struct UnaryExpression{
    Operand oper;
    Expression* expr;
};

struct Assignment{
    std::string name;
    Operand oper;
    Expression* expr;
};

class Expression{
    private:
        std::variant<std::string, BinaryExpression, UnaryExpression, Assignment> expr;
        ExprType type;
    public:
        Expression(Expression* l, Operand oper, Expression* r): expr(BinaryExpression{.l = l, .oper = oper, .r = r}), type(ExprType::BinaryExpr) {};
        Expression(Operand oper, Expression* r): expr(UnaryExpression{oper, r}), type(ExprType::UnaryExpr) {};
        Expression(std::string name, Operand oper, Expression* expr): expr(Assignment{name, oper, expr}), type(ExprType::Assignment) {};
        Expression(std::string name): expr(name), type(ExprType::Var) {};
        VariableValue evaluate(GameState* gameState);
};

#endif