#include "button.hpp"

std::string Button::getDisplay(GameState& gameState){
    std::string display = "";
    for(size_t i=0; i<this->display.size(); i++){
        if (!this->display[i].isConditionTrue(gameState))
            continue;
        for(auto& chunk : this->display[i].displayLine.chunks){
            display += chunk.getDisplay(gameState);
        }
        return display;
    }
    return display;
}

void Button::setDisplay(std::unique_ptr<Node> condition, std::string t){
    bool inTextState = true;
    bool inQuotations = false;
    std::string tmp = "";

    DisplayLine line;

    for(size_t i=0; i<t.size(); i++){
        if (t[i] == '\"'){
            if (inQuotations) inQuotations = false;
            else inQuotations = true;
        }
        else if (t[i] == '{' && !inQuotations){
            line.appendTextChunk(tmp);
            tmp.clear();
            inTextState = false;
        }
        else if (t[i] == '}' && !inQuotations){
            Variable* var = Defs::getVariable(tmp);
            if (var != nullptr){
                var->addButtonAsDisplay(this);
                line.appendVarChunk(tmp);
            }
            tmp.clear();
            inTextState = true;
        }
        else
            tmp += t[i];
    }

    if (!tmp.empty()){
        if (inTextState)
            line.appendTextChunk(tmp);
        else
            line.appendVarChunk(tmp);
    }

    display.emplace_back(line, std::move(condition));
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