#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"
#include "term.hpp"
#include "button.hpp"
#include "raylib.h"
#include <set>

int main(){
    InitWindow(800, 450, "test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("If you see this, Raylib is working!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    Variable x ("x",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("1"))),        VariableValue(0.0));
    Variable y ("y",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("1"))),        VariableValue(5.0));
    Variable s ("s",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("x >= 0"))),   VariableValue(5.0));
    Variable mq("mq", getScoreParams(25, 0, Polarity::Normal),  std::move(construct(tokenize("x*y >= 0"))), VariableValue(5.0));

    GameState gameState;
    gameState.addVariable(&x);
    gameState.addVariable(&y);
    gameState.addVariable(&s);
    gameState.addVariable(&mq);
    gameState.updateVariables();
    gameState.printUnlocked();
    std::vector<Token> v = tokenize("s = 10 * if[mq == 4, 5]"/*s -= 1/((mq+1)+0.1*~x)*/);
    for (size_t i=0; i<v.size(); i++){
        v[i].print();
    }

    auto p = construct(v);
    std::cout << "evaluated: " << p->evaluate(gameState).getAsDouble() << std::endl;
    std::cout << "insight: " << p->insight(gameState, 0) << std::endl;

    

    // p->simulate(gameState).add(p->simulate(gameState)).print();

    Term term("testing term");
    term.setCondition(construct(tokenize("x == 2 ")));
    term.addExpression(construct(tokenize("mq += _NR*2000 ")));
    term.addExpression(construct(tokenize("mq += _R*2000 ")));
    term.addExpression(construct(tokenize("s -= 1/(~mq+0.1*~x) ")));
    term.addExpression(construct(tokenize("if[_NR>0.4, x+=1]")));

    std::unique_ptr<Node> expr = construct(tokenize("((_R)*(_R) + 4)^2"));
    std::cout << "distribution insight: " << expr->getRandomDistribution()->arithmeticalInsight() << std::endl;
    auto min = expr->getRangeObject();
    std::cout << "tokenized" << std::endl;
    std::cout << "min is " << expr->getRangeObject().min->evaluate(gameState).getAsDouble() << std::endl;
    std::cout << "max is " << expr->getRangeObject().max->evaluate(gameState).getAsDouble() << std::endl;

    std::cout << "random number between " << expr->getRangeObject().min->insight(gameState, 0) << " and " << expr->getRangeObject().max->insight(gameState, 0) << std::endl;

    auto& ss = term.getInputs();
    for (const auto& varName : ss) {
        std::cout << "input: " << varName << std::endl;
    }

    ss = term.getDependencies();
    for (const std::string& varName : ss) {
        std::cout << "dependencies: " << varName << std::endl;
    }

    std::cout << "is condition met? " << term.isConditionMet(gameState) << std::endl;
    VariableChanges changes = term.simulate(gameState);
    printf("changes: ");
    std::cout << changes.insight() << std::endl;
    printf("\n");
    gameState.applyChanges(changes);
    std::cout << "is term unlocked? " << term.isUnlocked(gameState) << std::endl;

    std::cout << term.isUnlocked(gameState) << std::endl;

    for(size_t i = 0; i < 200; i++){
        term.insight(gameState);
    }

    std::cout << term.insight(gameState) << std::endl;

    //term.insight()

    auto expr2 = construct(tokenize("x += _R*_R*4"));

    std::cout << expr2->insight(gameState, 0) << "\n";

    gameState.forceRandom(0.5);
    std::cout << gameState.getVarValueAsDouble("_R") << std::endl;
    std::cout << gameState.getVarValueAsDouble("_NR") << std::endl;
    std::cout << gameState.getVarValue("_R").getAsDouble() << std::endl;
    std::cout << gameState.getVarValue("_NR").getAsDouble() << std::endl;
    std::cout << expr2->simulate(gameState).insight() << "\n";
    gameState.freeRandom();

    Button b;
    b.setDisplay("{x} / {y}");
    std::cout << "bdisplay: " << b.getDisplay(gameState) << std::endl;

    CloseWindow();

    return 0;
}