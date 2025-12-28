#include <iostream>
#include "game_state.hpp"
#include "expression.hpp"

int main(){
    std::cout << "Henlo" << std::endl;
    Variable var("x", VariableValue(100), {100, 0, Polarity::Normal});
    var.add(-50);
    std::cout << var.getScore() << std::endl;

    GameState gameState;
    gameState.getVar("x")->add(12);
    std::cout << gameState.getScore() << std::endl;

    Expression x("x");
    Expression y("y");
    Expression expr(&x, Operand::Divide, &y);
    std::cout << expr.evaluate(&gameState).getAsDouble() << std::endl;

    return 0;
}