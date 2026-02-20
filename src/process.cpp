#include "process.hpp"
#include "game_state.hpp"

std::vector<DisplayLine> Process::insight(GameState& gameState, int level){
    return {};
}

double Process::getInterval(GameState& gameState) const{
    return interval->evaluate(gameState);
}


bool Process::isStartConditionMet(GameState& gameState) const{
    return startCondition->evaluate(gameState) != 0;
}

bool Process::isEndConditionMet(GameState& gameState) const{
    return endCondition->evaluate(gameState) != 0;
}

bool Process::isLocked(GameState& gameState) const{
    for(const auto& term : terms){
        if (!term->isLocked(gameState))
            return false;
    }
    return true;
}

bool Process::isActive(GameState& gameState) const{
    for(const auto& term : terms){
        if (term->isActive(gameState))
            return true;
    }
    return false;
}

bool Process::isBlocked(GameState& gameState) const{
    for(const auto& term : terms){
        if (!term->isBlocked(gameState))
            return false;
    }
    return true;
}

void Process::setStartCondition(std::unique_ptr<Node> startCondition){
    this->startCondition = std::move(startCondition);
}

void Process::setEndCondition(std::unique_ptr<Node> endCondition){
    this->endCondition = std::move(endCondition);
}

void Process::setInterval(std::unique_ptr<Node> interval){
    this->interval = std::move(interval);
}


void Process::addTerm(std::unique_ptr<Term> term){
    term->setParent(*this);
    //term->updateSets();
    //term->printSets();
    terms.push_back(std::move(term));
}

const std::vector<Expression> Process::getExpressions(GameState& gameState) const{
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

std::set<std::string> Process::getInputs(bool root, std::string function) const{
    std::set<std::string> inputs = {};
    for (const auto& term : terms){
        auto termInputs = term->getInputs(true, function);
        inputs.insert(termInputs.begin(), termInputs.end());
    }
    return inputs;
}

void Process::bind(){
    for(auto& term : terms){
        term->bind();
    }
};