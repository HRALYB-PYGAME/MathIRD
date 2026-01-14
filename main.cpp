#include <iostream>
#include "game_state.hpp"
#include "expressiontree.hpp"
#include "term.hpp"
#include "button.hpp"
#include "raylib.h"
#include "insightable.hpp"
#include <set>

void drawInsight(const std::vector<DisplayLine>& lines, Vector2 startPos, GameState& gameState){
    const auto linesToDraw = lines;  // Copy to protect iteration from modifications
    Vector2 cursor = startPos;
    constexpr float fontSize = 20.0f;
    constexpr float spacing = 2.0f;
    constexpr float wordGap = 10.0f;

    for(const auto& line : linesToDraw){
        cursor.x += line.indent;
        for(const auto& chunk : line.chunks){
            if (chunk.val.empty() && chunk.type != DisplayType::NewLine) continue;
            
            switch(chunk.type){
                case DisplayType::Text: {
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), chunk.val.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() + 10){
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
                    DrawTextEx(GetFontDefault(), chunk.val.c_str(), cursor, fontSize, spacing, textColor);
                    cursor.x += wordSize.x + wordGap;
                    break;
                }
                case DisplayType::Var: {
                    std::string text = formatDouble(gameState.getVarValueAsDouble(chunk.val));
                    Vector2 wordSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
                    if (cursor.x + wordSize.x >= GetScreenWidth() + 10){
                        cursor.x = 100;
                        cursor.y += 25;
                    }
                    DrawTextEx(GetFontDefault(), text.c_str(), cursor, fontSize, spacing, RAYWHITE);
                    cursor.x += wordSize.x + wordGap;
                    break;
                }
                case DisplayType::NewLine: {
                    cursor.y += 25;
                    break;
                }
            }
        }
        cursor.y += 25;
    }
}

int main(){
    InitWindow(800, 450, "test");
    Vector2 cursor = { 100, 100 };
    float fontSize = 20.0f;
    float spacing = 2.0f;
    float wordGap = 10.0f;
    auto color = BLACK;
    Variable x ("x",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("1"))),        VariableValue(0.0));
    Variable y ("y",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("1"))),        VariableValue(5.0));
    Variable s ("s",  getScoreParams(100, 0, Polarity::Normal), std::move(construct(tokenize("x >= 0"))),   VariableValue(5.0));
    Variable mq("mq", getScoreParams(25, 0, Polarity::Normal),  std::move(construct(tokenize("x*y >= 0"))), VariableValue(5.0));

    GameState gameState;
    gameState.addVariable(&x);
    gameState.addVariable(&y);
    gameState.addVariable(&s);
    gameState.addVariable(&mq);

    std::unique_ptr<Node> expr = construct(tokenize("x+4 "));
    gameState.setCurrentInsight(expr->insight(gameState, 0));
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