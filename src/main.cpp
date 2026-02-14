#include "game_state.hpp"
#include <iostream>
#include <set>
#include <memory>
#include <chrono>

constexpr float fontSize = 20.0f;
constexpr float spacing = 2.0f;
constexpr float wordGap = 10.0f; // 5.0f for arithmetic
float currWordGap = 10.0f;
const short buttonsPerRow = 4;
const float outerPadding = 0.05; // percentage of the screen height/width
const float buttonPadding = 0.05; // percentage of button width to be used as a gap in between

Clock::time_point lastFrameTime;

Vector2 getCoordsFromButtonPosition(const ButtonPosition& buttonPos, double& buttonSize, bool getCenter){
    double screenWidth = GetScreenWidth();
    double buttonSectionWidth = (screenWidth/2)-outerPadding*screenWidth;
    double buttonWidth = buttonSectionWidth/(buttonsPerRow + buttonPadding*(buttonsPerRow-1));

    Vector2 coords = { 0,0 };
    coords.x = (buttonPos.col)*(buttonWidth + buttonPadding*buttonWidth) + outerPadding*screenWidth;
    coords.y = (buttonPos.row)*(buttonWidth + buttonPadding*buttonWidth) + outerPadding*screenWidth;

    if (getCenter){
        coords.x += buttonWidth/2;
        coords.y += buttonWidth/2;
    }

    buttonSize = buttonWidth;

    return coords;
}

void drawButtons(GameState& gameState){
    for(auto& [key, button] : Defs::btns){
        ButtonPosition buttonPos = button.getPosition();
        const char* displayText = button.getDisplay(gameState).c_str();
        double buttonWidth;

        Vector2 pos = getCoordsFromButtonPosition(buttonPos, buttonWidth, false);
        Rectangle rect = { pos.x, pos.y, buttonWidth, buttonWidth };
        DrawRectanglePro(rect, {0,0}, 0, GREEN);

        Vector2 textDimensions = MeasureTextEx(GetFontDefault(), displayText, fontSize, spacing);
        Vector2 textOffset = { (buttonWidth-textDimensions.x)/2, (buttonWidth-textDimensions.y)/2 };
        DrawText(displayText, pos.x+textOffset.x, pos.y+textOffset.y, fontSize, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), rect)){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                LOG("main.cpp\tdrawButtons()" << button.getName() << " LEFT CLICK");
                auto packets = gameState.generatePackets(&button);
                for(auto& packet : packets) LOG("main.cpp\tdrawButtons() packetvar=" << packet.variable);
                gameState.sendPackets(packets);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                LOG("main.cpp\tdrawButtons()" << button.getName() << " RIGHT CLICK");
                gameState.setCurrentInsightable(&button);
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
                            gameState.setCurrentInsightable(chunk.link);
                    }
                    DrawTextEx(GetFontDefault(), chunk.text.c_str(), cursor, fontSize, spacing, textColor);
                    cursor.x += wordSize.x + currWordGap;
                    break;
                }
                case DisplayType::Var: {
                    std::string text = formatDouble(gameState.getVarValue(chunk.text));
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
                    cursor.x = startPos.x;
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
        cursor.x = startPos.x;
    }
}

void updatePackets(GameState& gameState){
    auto now = Clock::now();
    auto& packets = gameState.getPackets();
    while(!packets.empty() && packets.back().getProgress(gameState.getInGameTime()) >= 1){
        auto& packet = packets.back();
        double newValue = packet.expression.evaluate(gameState);
        for(auto lock : packet.expression.variableLocks){
            gameState.unblockVariable(lock);
        }
        LOG("main.cpp\tupdatePackets() PACKET TO BE EATEN (var=" << packet.variable << ", newValue=" << newValue << ")");
        gameState.setVarValue(packet.variable, newValue);
        packets.pop_back();
        LOG("main.cpp\tupdatePackets() PACKET EATEN (var=" << packet.variable << ", newValue=" << newValue << ") GAMESTATE packet length=" << packets.size());
    }
}

void updateProcesses(GameState& gameState){
    auto& calendar = gameState.getCalendar();
    while(!calendar.empty() && calendar.back().time < gameState.getInGameTime()){
        auto& entry = calendar.back();
        switch(entry.type){
            case CalendarEntryType::ProcessTick:
                std::string processName = entry.process->getName();
                auto packets = gameState.generatePackets(entry.process, ButtonPosition(0, 5));
                gameState.sendPackets(packets);

                gameState.updateProcesses();
                if(gameState.isProcessActive(entry.process->getName())){
                    double interval = entry.process->getInterval(gameState);
                    gameState.addNewProcessEvent(entry.process, interval);
                }
                break;
        }

        calendar.pop_back();
    }
}

void drawPackets(GameState& gameState){
    for(auto& packet :gameState.getPackets()){
        double buttonWidth;
        Vector2 startCoords = getCoordsFromButtonPosition(packet.startPos, buttonWidth, true);
        Vector2 endCoords = getCoordsFromButtonPosition(packet.endPos, buttonWidth, true);
        double progress = packet.getProgress(gameState.getInGameTime());

        Vector2 currentCoords = { startCoords.x + (endCoords.x - startCoords.x)*progress,
                                  startCoords.y + (endCoords.y - startCoords.y)*progress };

        DrawCircle(currentCoords.x, currentCoords.y, packet.radius, packet.color);
    }
}

int main(int argc, char** argv){
    // var to btn

    InitWindow(800, 450, "MathIRD");
    Vector2 cursor = { 100, 100 };
    auto color = BLACK;

    // LOAD ASSETS
    std::unordered_map<std::string, std::string> linkerMap;
    Defs::loadVariables("assets/variables", linkerMap);

    Defs::loadButtons("assets/buttons");
    Defs::linkVariableHomeButtons(linkerMap);

    Defs::loadProcesses("assets/processes");

    GameState gameState;
    gameState.init();

    lastFrameTime = Clock::now();

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        gameState.addSecondsToInGameTime(durationToSeconds(Clock::now()-lastFrameTime));
        lastFrameTime = Clock::now();
        BeginDrawing();
        ClearBackground(color);

        cursor = { (float)GetScreenWidth()/2, 35 };
        drawInsight(gameState.currentInsight, cursor, gameState);
        drawButtons(gameState);
        updatePackets(gameState);
        updateProcesses(gameState);
        drawPackets(gameState);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}