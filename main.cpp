#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"

int main(){
    std::cout << "Henlo" << std::endl;
    Variable var("x", VariableValue(100), {100, 0, Polarity::Normal});
    var.add(-50);
    std::cout << var.getScore() << std::endl;

    GameState gameState;
    gameState.getVar("x")->add(12);
    std::cout << gameState.getScore() << std::endl;

    // Expression x("x");
    // Expression y("y");
    // Expression expr(&x, Operand::Divide, &y);
    // std::cout << expr.evaluate(&gameState).getAsDouble() << std::endl;

    std::vector<Token> v = tokenize("-(x+3)*x ");
    for(int i=0; i<v.size(); i++){
        v[i].print();
    }

    auto c1 = std::make_unique<ConstantNode>(VariableValue(4));
    auto c2 = std::make_unique<ConstantNode>(VariableValue(4.5));
    auto o = std::make_unique<OperandNode>(Operand::Add, std::move(c1), std::move(c2));

    auto p = construct(v);
    std::cout << p->evaluate(gameState).getAsDouble() << std::endl;

    return 0;
}