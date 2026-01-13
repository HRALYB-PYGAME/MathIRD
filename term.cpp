#include "term.hpp"

bool Term::isConditionMet(GameState& gameState){
    return this->condition->evaluate(gameState).getAsBool();
}

VariableChanges Term::simulate(GameState& gameState){
    VariableChanges changes;
    GameState tmpState = gameState;
    for(size_t i=0; i<this->expressions.size(); i++){
        changes = changes.add(this->expressions[i]->simulate(tmpState));
        tmpState.applyChanges(changes);
    }
    return changes;
}

std::string Term::insight(GameState& gameState){
    gameState.setCurrentTerm(this->name);
    std::string insight;
    VariableChanges simulationResult = this->simulate(gameState);
    insight.append("Immediate changes:\n");
    insight.append(simulationResult.insight() + "\n");
    insight.append("if " + condition->insight(gameState, 0) + "\n");
    for(size_t i=0; i<expressions.size(); i++){
        insight.append(expressions[i]->insight(gameState, 0) + "\n");
        if (expressions[i]->oper == Operand::If && expressions[i]->isRangeObject())
            gameState.incrementCurrentIndex();
    }
    return insight;
}

void Term::setCondition(std::unique_ptr<Node> condition) {
    this->condition = std::move(condition);
    // updateDependencies();
}

void Term::addExpression(std::unique_ptr<Node> expression) {
    this->expressions.insert(this->expressions.end(), std::move(expression));
    // updateDependencies();
    // updateInputs();
}

void Term::updateSets(){
    updateDependencies();
    updateInputs();
    updateOutputs();
}

void Term::updateDependencies(){
    dependencies.clear();
    auto condDeps = condition->getDependencies();
    dependencies.insert(condDeps.begin(), condDeps.end());
    for (size_t i=0; i<expressions.size(); i++){
        auto exprDeps = expressions[i]->getDependencies();
        dependencies.insert(exprDeps.begin(), exprDeps.end());
    }
}

void Term::updateInputs(){
    inputs.clear();
    for (size_t i=0; i<expressions.size(); i++){
        auto exprIns = expressions[i]->getInputs(true);
        inputs.insert(exprIns.begin(), exprIns.end());
    }
}

void Term::updateOutputs(){
    outputs.clear();
    for (size_t i=0; i<expressions.size(); i++){
        auto exprOuts = expressions[i]->getOutputs(true);
        outputs.insert(exprOuts.begin(), exprOuts.end());
    }
}

bool Term::isUnlocked(GameState& gameState){
    for (const std::string& var : dependencies) {
        if (!gameState.isVariableUnlocked(var))
            return false;
    }
    return true;
}