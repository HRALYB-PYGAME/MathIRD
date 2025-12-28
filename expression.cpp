#include "expression.hpp"

VariableValue Expression::evaluate(GameState* gameState){
    if (this->type == ExprType::Var){
        Variable* var = gameState->getVar(std::get<std::string>(this->expr));
        if (var == nullptr) 
            return VariableValue(0.0);
        else 
            return var->getValue();
    }

    if (this->type == ExprType::BinaryExpr){
        BinaryExpression expr = std::get<BinaryExpression>(this->expr);
        Operand oper = expr.oper;

        VariableValue l = expr.l->evaluate(gameState);
        VariableValue r = expr.r->evaluate(gameState);

        switch(oper){
        case Operand::Add:
            return l.add(r);
        case Operand::Subtract:
            return l.subtract(r);
        case Operand::Multiply:
            return l.multiply(r);
        case Operand::Divide:
            return l.divide(r);
        }
    }
    //     case Operand::Modulo:
    //         return (double)((int)expr.l->evaluate(gameState) % (int)expr.r->evaluate(gameState));

    //     case Operand::Equal:
    //         return expr.l->evaluate(gameState) == expr.r->evaluate(gameState) ? 1.0 : 0.0;
    //     case Operand::NotEqual:
    //         return expr.l->evaluate(gameState) != expr.r->evaluate(gameState) ? 1.0 : 0.0;
    //     case Operand::Less:
    //         return expr.l->evaluate(gameState) < expr.r->evaluate(gameState) ? 1.0 : 0.0;
    //     case Operand::Greater:
    //         return expr.l->evaluate(gameState) > expr.r->evaluate(gameState) ? 1.0 : 0.0;
    //     case Operand::LessOrEqual:
    //         return expr.l->evaluate(gameState) <= expr.r->evaluate(gameState) ? 1.0 : 0.0;
    //     case Operand::GreaterOrEqual:
    //         return expr.l->evaluate(gameState) >= expr.r->evaluate(gameState) ? 1.0 : 0.0;

    //     case Operand::And:
    //         return expr.l->evaluate(gameState) && expr.r->evaluate(gameState) ? 1.0 : 0.0; 
    //     case Operand::Or:
    //         return expr.l->evaluate(gameState) || expr.r->evaluate(gameState) ? 1.0 : 0.0; 
    // }

    // if (this->type == ExprType::UnaryExpr){
    //     Operand oper = expr.oper;
    //     UnaryExpression expr = std::get<UnaryExpression>(this->expr);
    //     switch(oper){
    //     case Operand::Not:
    //         return expr.expr->evaluate(gameState) ? 0.0 : 1.0;
    //     }
    // }

    // if (this->type == ExprType::Assignment){
    //     Operand oper = expr.oper;
    //     Assignment expr = std::get<Assignment>(this->expr);
    //     double value = expr.expr->evaluate(gameState);
    //     switch(oper){
    //     case Operand::Assign:
    //         gameState->getVar(expr.name)->set(value);
    //         break;
    //     case Operand::AddAssign:
    //         gameState->getVar(expr.name)->add(value);
    //         break; 
    //     case Operand::SubAssign:
    //         break;
    //     case Operand::MulAssign:
    //         break;
    //     case Operand::DivAssign:
    //         break;
    //     }
    //     return value; 
    // }

    return 0.0;
}

bool isBinaryOperand(Operand oper){

}
bool isUnaryOperand(Operand oper){

}
bool isAssignment(Operand oper){

}