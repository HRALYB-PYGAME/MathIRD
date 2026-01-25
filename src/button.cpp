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
    LOG("button.cpp\tinsight() of " << name << " FUNCTION BEG");
    DisplayLine dl;
    dl.appendTextChunk("insight of " + name + " button");
    dl.appendNewLineChunk();
    dl.appendLines(simulate(gameState).insight(gameState, 0));
    dl.appendNewLineChunk();
    LOG("button.cpp\tinsight() of " << name << " returning insight");
    return {dl}; 
}

void Button::addTerm(std::unique_ptr<Term> term){
    term->setParent(*this);
    term->updateSets();
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
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " SIMULATION STARTED");
            VariableChanges c = term->simulate(gameState);
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " SIMULATION DONE");
            changes.add(c);
        }
        else
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " LOCKED");
    }
    return changes;
}