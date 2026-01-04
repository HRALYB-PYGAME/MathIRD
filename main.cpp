#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"
#include "term.hpp"

int main(){
    GameState gameState;
    gameState.addVariable("x", Variable("x", 0.0, getScoreParams(100, 0, Polarity::Normal)), std::move(construct(tokenize("1"))));
    gameState.addVariable("y", Variable("y", 5.0, getScoreParams(100, 0, Polarity::Normal)), std::move(construct(tokenize("1"))));
    gameState.addVariable("s", Variable("s", 5.0, getScoreParams(100, 0, Polarity::Normal)), std::move(construct(tokenize("x >= 0"))));
    gameState.addVariable("mq", Variable("mq", 5.0, getScoreParams(25, 0, Polarity::Normal)), std::move(construct(tokenize("x*y >= 1000"))));
    gameState.updateVariables();
    gameState.printUnlocked();
    std::vector<Token> v = tokenize("s -= 1/((mq+1)+0.1*~x) ");

    auto p = construct(v);
    std::cout << "evaluated: " << p->evaluate(gameState).getAsDouble() << std::endl;
    std::cout << "insight: " << p->insight(gameState, 0) << std::endl;

    p->simulate(gameState).add(p->simulate(gameState)).print();

    Term term;
    term.setCondition(construct(tokenize("x == 2 ")));
    term.addExpression(construct(tokenize("x += 200 ")));
    term.addExpression(construct(tokenize("s -= 1/(~mq+0.1*~x) ")));

    std::cout << "is condition met? " << term.isConditionMet(gameState) << std::endl;
    VariableChanges changes = term.simulate(gameState);
    changes.print();
    gameState.applyChanges(changes);
    std::cout << "is condition met? " << term.isConditionMet(gameState) << std::endl;

    return 0;
}