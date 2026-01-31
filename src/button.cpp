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
    std::vector<DisplayLine> dl;
    DisplayLine line;
    line.appendTextChunk("insight of " + name + " button");
    line.appendNewLineChunk();
    line.appendTextChunk("IMMEDIATE CHANGES");
    line.appendNewLineChunk();
    line.appendLines(simulate(gameState).insight(gameState, 0));
    line.appendNewLineChunk();
    dl.push_back(line);

    for(auto& term : terms){
        line.clear();
        line.appendTextChunk("Term ");
        DisplayChunk termChunk(term->getName());
        termChunk.setLink(term.get());
        line.appendChunk(termChunk);
        dl.push_back(line);

        auto termInsight = term->insight(gameState, 0);
        dl.insert(dl.end(), termInsight.begin(), termInsight.end());
        dl.push_back({});
    }
    LOG("button.cpp\tinsight() of " << name << " returning insight");
    return dl;
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
        if (term->isUnlocked(gameState) && term->isConditionMet(gameState)){
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

const std::vector<std::unique_ptr<Node>> Button::getExpressions(GameState& gameState) const{
    std::vector<std::unique_ptr<Node>> result;
    for(const auto& term : terms){
        if (term->isUnlocked(gameState) && term->isConditionMet(gameState)){
            for(const auto& expr : term->getExpressions()) {
                result.push_back(expr->clone()); 
            }
        }
    }
    return result;
}