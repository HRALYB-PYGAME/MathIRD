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
            Variable* var = Defs::getVariable(tmp);
            if (var != nullptr){
                var->addButtonAsDisplay(this);
                display.insert(display.end(), DisplayChunk(tmp, DisplayType::Var));
            }
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
    DisplayLine dl;
    bool isXUnlocked = gameState.isVariableUnlocked("x");
    dl.appendTextChunk(isXUnlocked ? "ano" : "ne ):");
    return {dl}; 
}

void Button::addTerm(std::unique_ptr<Term> term){
    term->setParent(*this);
    terms.push_back(std::move(term));
}

bool Button::isUnlocked(GameState& gameState){
    for(auto& term : terms){
        if (term->isUnlocked(gameState))
            return true;
    }
    return false;
}

VariableChanges Button::simulate(GameState& gameState){
    VariableChanges changes;
    for(auto& term : terms){
        if (term->isUnlocked(gameState)){
            std::cout << "unlocked\n";
            VariableChanges c = term->simulate(gameState);
            std::cout << "return val\n";
            c.insight(gameState, 0);
            changes.add(c);
        }
        else
            std::cout << "locked\n";
    }
    std::cout << "button ci\n";
    changes.insight(gameState, 0);
    return changes;
}