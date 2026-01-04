#include "term.hpp"

bool Term::isConditionMet(GameState& gameState){
    return this->condition->evaluate(gameState).getAsBool();
}

VariableChanges Term::simulate(GameState& gameState){
    VariableChanges changes;
    GameState tmpState = gameState;
    for(int i=0; i<this->expressions.size(); i++){
        changes = changes.add(this->expressions[i]->simulate(tmpState));
        tmpState.applyChanges(changes);
    }
    return changes;
}

void Term::setCondition(std::unique_ptr<Node> condition) {
    this->condition = std::move(condition);
}

void Term::addExpression(std::unique_ptr<Node> expression) {
    this->expressions.insert(this->expressions.end(), std::move(expression));
}

bool Term::isUnlocked(GameState& gameState){
    // get all variables thats dependant
    // check if all are unlocked

    // maybe the variables should be stored somewhere
}