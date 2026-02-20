#include "button.hpp"
#include "game_state.hpp"

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
    auto res = gameState.predict(this);

    std::vector<DisplayLine> dl;
    DisplayLine line;

    for(auto& [source, change] : res){
        line.appendTextChunk(source.sourceName + " (" + std::to_string(source.termIndex) + ", " + std::to_string(source.exprIndex) + ")");
        dl.push_back(line);
        line.clear();
        line.appendLines(change.insight(gameState, 0));
        dl.push_back(line);
        line.clear();
    }

    LOG("button.cpp\tinsight() of " << name << " FUNCTION BEG");
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
    LOG("adding new term");
    if (term == nullptr) LOG("adding nullptr term??");
    term->setParent(*this);
    LOG("parent done");
    
    LOG("sets done");
    terms.push_back(std::move(term));
    LOG("term added");
}

bool Button::isLocked(GameState& gameState) const{
    for(auto& term : terms){
        if (!term->isLocked(gameState))
            return false;
    }
    return true;
}

bool Button::isActive(GameState& gameState) const{
    for(auto& term : terms){
        if (term->isActive(gameState))
            return true;
    }
    return false;
}

bool Button::isBlocked(GameState& gameState) const{
    for(auto& term : terms){
        if (!term->isBlocked(gameState))
            return false;
    }
    return true;
}

VariableChanges Button::simulate(GameState& gameState){
    VariableChanges changes;
    for(auto& term : terms){
        auto state = term->getState(gameState);
        if (state == InsightableState::Unblocked){
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " SIMULATION STARTED");
            VariableChanges c = term->simulate(gameState);
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " SIMULATION DONE");
            changes.add(c);
        }
        else if (state == InsightableState::Locked)
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " LOCKED");
        else if (state == InsightableState::Inactive)
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " NOT ACTIVE");
        else
            LOG("button.cpp\tsimulate() of " << name << " TERM " << term->getName() << " BLOCKED");
    }
    return changes;
}

const std::vector<Expression> Button::getExpressions(GameState& gameState) const{
    std::vector<Expression> result;
    for(const auto& term : terms){
        if (term->getState(gameState) == InsightableState::Unblocked){
            for(const auto& expr : term->getExpressions()) {
                result.push_back(expr.clone()); 
            }
        }
    }
    return result;
}

std::set<std::string> Button::getInputs(bool root, std::string function) const{
    std::set<std::string> inputs = {};
    for (const auto& term : terms){
        auto termInputs = term->getInputs(true, function);
        inputs.insert(termInputs.begin(), termInputs.end());
    }
    return inputs;
}