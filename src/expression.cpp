#include "expressiontree.hpp"
#include "game_state.hpp"

void Expression::normalize(){
    Node* newExpr = expr->normalize(false);
    if (newExpr != expr.get()) {
        expr.release();
        expr.reset(newExpr);
    }
}

Expression Expression::clone() const{
    return Expression(expr ? expr->clone() : nullptr, variableLocks);
}