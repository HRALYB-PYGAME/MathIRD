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
    updateDependencies();
}

void Term::addExpression(std::unique_ptr<Node> expression) {
    this->expressions.insert(this->expressions.end(), std::move(expression));
    updateDependencies();
}

void Term::updateDependencies(){
    dependencies.clear();
    auto condDeps = condition->getDependencies();
    dependencies.insert(condDeps.begin(), condDeps.end());
    for (int i=0; i<expressions.size(); i++){
        auto exprDeps = expressions[i]->getDependencies();
        dependencies.insert(exprDeps.begin(), exprDeps.end());
    }
}

bool Term::isUnlocked(GameState& gameState){
    for (const std::string& var : dependencies) {
        if (!gameState.isVariableUnlocked(var))
            return false;
    }
    return true;
    // get all variables thats dependant
    // check if all are unlocked

    // maybe the variables should be stored somewhere
}