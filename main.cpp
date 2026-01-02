#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"
#include "term.hpp"

int main(){
    std::cout << "Henlo" << std::endl;
    Variable var("x", VariableValue(100), {100, 0, Polarity::Normal});
    var.add(-50);
    std::cout << var.getScore() << std::endl;

    GameState gameState;
    gameState.getVar("x")->add(2);
    std::cout << gameState.getScore() << std::endl;

    // Expression x("x");
    // Expression y("y");
    // Expression expr(&x, Operand::Divide, &y);
    // std::cout << expr.evaluate(&gameState).getAsDouble() << std::endl;

    std::vector<Token> v = tokenize("s -= 1/(mq+0.1*x) ");

    auto p = construct(v);
    std::cout << "evaluated: " << p->evaluate(gameState).getAsDouble() << std::endl;
    std::cout << "insight: " << p->insight(gameState, 0) << std::endl;

    printf("\x1B[31mTexting\033[0m\t\t");

    p->simulate(gameState).add(p->simulate(gameState)).print();

    Term term;
    term.setCondition(construct(tokenize("x == 2 ")));
    term.addExpression(construct(tokenize("x += 200 ")));
    term.addExpression(construct(tokenize("s -= 1/(mq+0.1*x) ")));

    std::cout << "is condition met? " << term.isConditionMet(gameState) << std::endl;
    VariableChanges changes = term.simulate(gameState);
    changes.print();
    gameState.applyChanges(changes);
    std::cout << "is condition met? " << term.isConditionMet(gameState) << std::endl;

    return 0;
}