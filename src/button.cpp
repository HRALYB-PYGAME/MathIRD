#include "button.hpp"

std::string Button::getDisplay(GameState& gameState){
    std::string display = "";
    for(size_t i=0; i<this->display.size(); i++){
        display += this->display[i].getDisplay(gameState);
    }
    return display;
}

void Button::setDisplay(std::string t){
    bool inTextState = true;
    bool inQuotations = false;
    std::string tmp = "";

    for(size_t i=0; i<t.size(); i++){
        if (t[i] == '\"'){
            if (inQuotations) inQuotations = false;
            else inQuotations = true;
        }
        else if (t[i] == '{' && !inQuotations){
            display.insert(display.end(), DisplayChunk(tmp, DisplayType::Text));
            tmp.clear();
            inTextState = false;
        }
        else if (t[i] == '}' && !inQuotations){
            display.insert(display.end(), DisplayChunk(tmp, DisplayType::Var));
            tmp.clear();
            inTextState = true;
        }
        else
            tmp += t[i];
    }

    if (!tmp.empty()){
        if (inTextState)
            display.insert(display.end(), DisplayChunk(tmp, DisplayType::Text));
        else
            display.insert(display.end(), DisplayChunk(tmp, DisplayType::Var));
    }
}

std::vector<DisplayLine> Button::insight(GameState& gameState, int level) {
    return {}; 
}