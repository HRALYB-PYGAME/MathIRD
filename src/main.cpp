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
const short buttonsPerRow = 4;
const float outerPadding = 0.05; // percentage of the screen height/width
const float buttonPadding = 0.05; // percentage of button width to be used as a gap in between

void drawButtons(GameState& gameState){
    Vector2 startPos = {outerPadding*GetScreenWidth(), outerPadding*GetScreenHeight()};

    float buttonWidth = (GetScreenWidth()/2-2*GetScreenWidth()*outerPadding)/buttonsPerRow;

    for(auto& [key, button] : Defs::btns){
        ButtonPosition buttonPos = button.getPosition();
        Vector2 relativePos = {buttonWidth*buttonPos.row, buttonWidth*buttonPos.col};
        Rectangle rect = {startPos.x + relativePos.x, startPos.y + relativePos.y, buttonWidth*(1-buttonPadding), buttonWidth*(1-buttonPadding)};
        DrawRectanglePro(rect, {0,0}, 0, GREEN);
        DrawText(button.getDisplay(gameState).c_str(), startPos.x + relativePos.x + buttonWidth/2, startPos.y + relativePos.y + buttonWidth/2, fontSize, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), rect)){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                VariableChanges c = button.simulate(gameState);
                c.insight(gameState, 0);
                gameState.applyChanges(c);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){

            } 
        }
    }
    
}

void drawInsight(const std::vector<DisplayLine>& lines, Vector2 startPos, GameState& gameState){
    const auto linesToDraw = lines;  // Copy to protect iteration from modifications
    Vector2 cursor = startPos;

    for(const auto& line : linesToDraw){
        cursor.x += line.indent;
        for(const auto& chunk : line.chunks){            
            switch(chunk.type){
                case DisplayType::Text: {
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), chunk.text.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() - 10){
                        cursor.x = startPos.x;
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
                    //DrawRectangle(wordRect.x, wordRect.y, wordRect.width + currWordGap + 5, wordRect.height, GREEN);
                    DrawTextEx(GetFontDefault(), chunk.text.c_str(), cursor, fontSize, spacing, textColor);
                    cursor.x += wordSize.x + currWordGap;
                    break;
                }
                case DisplayType::Var: {
                    std::string text = formatDouble(gameState.getVarValueAsDouble(chunk.text));
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() - 10){
                        cursor.x = startPos.x;
                        cursor.y += 25;
                    }
                    DrawTextEx(GetFontDefault(), text.c_str(), cursor, fontSize, spacing, RAYWHITE);     
                    cursor.x += wordSize.x + currWordGap;               
                    break;
                }
                case DisplayType::NewLine: {
                    cursor.y += 25;
                    break;
                }
                case DisplayType::Indent: {
                    cursor.x += chunk.scalar;
                    if (cursor.x >= GetScreenWidth() - 10){
                        cursor.x = startPos.x;
                        cursor.y += 25;
                    }
                    break;
                }
                case DisplayType::WordGap: {
                    currWordGap = wordGap*chunk.scalar;
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
    std::unique_ptr<Node> expr = construct(tokenize("x=x-1 "));
    std::unique_ptr<Node> cond = construct(tokenize("1"));

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
    vc.add("x", 0, 5, 2);
    std::cout << "length of changes: " << vc.changes.size() << std::endl;

    Button b("testbtn");
    b.addTerm(std::make_unique<Term>(std::move(term)));
    b.setDisplay("{x}");
    Defs::addButton(std::move(b));

    gameState.updateVariableSets(&term);

    Defs::getVariable("y")->printDependencies();

    Button* testbtn = Defs::getButton("testbtn");
    testbtn->setPosition(2,0);

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

    std::cout << "btn list start\n";
    for(auto& [name, entry] : Defs::btns){
        std::cout << "\"" << name << "\": \"" << entry.getName() << "\"" << std::endl;
    }
    std::cout << "btn list end\n";

    Button* tb = Defs::getButton("testbtn");
    if (tb == nullptr) std::cout << "?\n";

    for (auto& [name, var] : Defs::vars){
        gameState.addVariable(&var);
    }
    gameState.updateVariables();

    std::vector<DisplayLine> i = tb->insight(gameState, 0);
    gameState.setCurrentInsight(i);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(color);
        cursor = { (float)GetScreenWidth()/2, 35 };
        drawInsight(gameState.currentInsight, cursor, gameState);
        drawButtons(gameState);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}