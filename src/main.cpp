#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"
#include "term.hpp"
#include "button.hpp"
#include "raylib.h"
#include "insightable.hpp"
#include <set>
#include <memory>

constexpr float fontSize = 20.0f;
constexpr float spacing = 2.0f;
constexpr float wordGap = 10.0f; // 5.0f for arithmetic
float currWordGap = 10.0f;

void drawInsight(const std::vector<DisplayLine>& lines, Vector2 startPos, GameState& gameState){
    const auto linesToDraw = lines;  // Copy to protect iteration from modifications
    Vector2 cursor = startPos;
    //std::cout << currWordGap << std::endl;

    for(const auto& line : linesToDraw){
        cursor.x += line.indent;
        for(const auto& chunk : line.chunks){            
            switch(chunk.type){
                case DisplayType::Text: {
                    cursor.x += currWordGap;
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), chunk.text.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() - 10){
                        cursor.x = 100;
                        cursor.y += 25;
                    }
                    Rectangle wordRect = { cursor.x, cursor.y, wordSize.x, wordSize.y };
                    
                    Color textColor = RAYWHITE;
                    if (!chunk.hover.empty() && CheckCollisionPointRec(GetMousePosition(), wordRect)) {
                        textColor = GREEN;
                        if (cursor.y > 0)
                            drawInsight({DisplayLine(chunk.hover)}, { cursor.x, cursor.y - 25 }, gameState);
                    }
                    if (chunk.link != nullptr && CheckCollisionPointRec(GetMousePosition(), wordRect)) {
                        textColor = BLUE;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                            gameState.setCurrentInsight(chunk.link->insight(gameState, 0));
                    }
                    DrawTextEx(GetFontDefault(), chunk.text.c_str(), cursor, fontSize, spacing, textColor);
                    cursor.x += wordSize.x;
                    break;
                }
                case DisplayType::Var: {
                    cursor.x += currWordGap;
                    std::string text = formatDouble(gameState.getVarValueAsDouble(chunk.text));
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() - 10){
                        cursor.x = 100;
                        cursor.y += 25;
                    }
                    DrawTextEx(GetFontDefault(), text.c_str(), cursor, fontSize, spacing, RAYWHITE);     
                    cursor.x += wordSize.x;               
                    break;
                }
                case DisplayType::NewLine: {
                    cursor.y += 25;
                    break;
                }
                case DisplayType::Indent: {
                    cursor.x += chunk.scalar;
                    if (cursor.x >= GetScreenWidth() - 10){
                        cursor.x = 100;
                        cursor.y += 25;
                    }
                    break;
                }
                case DisplayType::WordGap: {
                    double temp = currWordGap;
                    currWordGap = wordGap*chunk.scalar;
                    if (temp > currWordGap) cursor.x += temp-currWordGap;
                    break;
                }
            }
        }
        cursor.y += 25;
    }
}

int main(int argc, char** argv){
    std::cout << argv[0] << std::endl;

    // var to btn

    InitWindow(800, 450, "MathIRD");
    Vector2 cursor = { 100, 100 };
    auto color = BLACK;

    GameState gameState;

    // VARIABLES
    std::unordered_map<std::string, std::string> linkerMap;
    Defs::loadVariables("assets/variables", linkerMap);

    // FOLLOWING
    std::unique_ptr<Node> expr = construct(tokenize("y=(4+x+6)*_NR "));
    std::unique_ptr<Node> cond = construct(tokenize("mq>=0"));
    std::vector<DisplayLine> i = expr->insight(gameState, 0);
    gameState.setCurrentInsight(i);
    printLines(gameState.currentInsight);

    auto out = expr->getOutputs(true);
    std::cout << "outs:\n";
    for(auto o:out){
        std::cout << o << std::endl;
    }
    Term term("test");
    term.addExpression(std::move(expr));
    term.setCondition(std::move(cond));
    term.updateSets();

    VariableChanges vc;
    vc = vc.add("x", 0, 5, 2);
    std::cout << "length of changes: " << vc.changes.size() << std::endl;

    Button b("testbtn");
    b.addTerm(std::make_unique<Term>(std::move(term)));
    b.setDisplay("{x}");
    Defs::addButton(std::move(b));

    gameState.updateVariableSets(&term);

    Defs::getVariable("y")->printDependencies();

    Button* testbtn = Defs::getButton("testbtn");

    std::cout << "parent of terms\n";
    for(auto& i : testbtn->getTerms()){
        std::cout << i->getParent().getName() << std::endl;
    }

    std::cout << "x is displayed at buttons \n";
    for(auto i : Defs::getVariable("x")->displayedAtButtons){
        std::cout << i->getName() << std::endl;
    }

    Defs::linkVariableHomeButtons(linkerMap);

    std::cout << "x's home button \n";
    Button* homeBtn = Defs::getVariable("x")->getHomeButton();
    if (homeBtn != nullptr)
        std::cout << homeBtn->getName() << std::endl;
    else
        std::cout << "null homebtn\n";

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(color);
        cursor = { 100, 100 };
        drawInsight(gameState.currentInsight, cursor, gameState);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}