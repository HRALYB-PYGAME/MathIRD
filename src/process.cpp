#include "process.hpp"
#include "game_state.hpp"

std::vector<DisplayLine> Process::insight(GameState& gameState, int level){
    return {};
}

double Process::getInterval(GameState& gameState){
    return interval->evaluate(gameState);
}


bool Process::isStartConditionMet(GameState& gameState){
    return startCondition->evaluate(gameState) != 0;
}

bool Process::isEndConditionMet(GameState& gameState){
    return endCondition->evaluate(gameState) != 0;
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
    term->updateSets();
    term->printSets();
    terms.push_back(std::move(term));
}

const std::vector<Expression> Process::getExpressions(GameState& gameState) const{
    std::vector<Expression> result;
    for(const auto& term : terms){
        if (term->isUnblocked(gameState)){
            for(const auto& expr : term->getExpressions()) {
                result.push_back(expr.clone()); 
            }
        }
    }
    return result;
}