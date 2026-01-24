#include "term.hpp"

bool Term::isConditionMet(GameState& gameState){
    return this->condition->evaluate(gameState).getAsBool();
}

VariableChanges Term::simulate(GameState& gameState){
    VariableChanges changes;
    GameState tmpState = gameState;
    if (!this->condition->evaluate(gameState).getAsBool())
        return changes;
    for(size_t i=0; i<this->expressions.size(); i++){
        std::cout << "expression\n";
        changes.add(this->expressions[i]->simulate(tmpState));
        tmpState.applyChanges(changes);
    }
    std::cout << "term ci\n";
    changes.insight(gameState, 0);
    return changes;
}

std::vector<DisplayLine> Term::insight(GameState& gameState, int level){
    gameState.setCurrentTerm(this->name);
    std::vector<DisplayLine> lines;
    std::vector<DisplayChunk> chunks;
    VariableChanges simulationResult = this->simulate(gameState);

    /*chunks.push_back(DisplayChunk("Immediate changes:", DisplayType::Text));
    lines.insert(lines.back(), chunks.begin(), chunks.back());
    std::vector<DisplayChunk> simulationChunks = simulationResult.insight(gameState, level);
    chunks.insert(chunks.end(), simulationChunks.begin(), simulationChunks.end());
    chunks.push_back(DisplayChunk("\n", DisplayType::Text));

    chunks.push_back(DisplayChunk("If ", DisplayType::Text));
    std::vector<DisplayChunk> conditionChunks = condition->insight(gameState, level);
    chunks.insert(chunks.end(), conditionChunks.begin(), conditionChunks.end());
    chunks.push_back(DisplayChunk("\n", DisplayType::Text));

    for(size_t i=0; i<expressions.size(); i++){
        std::vector<DisplayChunk> expressionChunks = expressions[i]->insight(gameState, level);
        chunks.insert(chunks.end(), expressionChunks.begin(), expressionChunks.end());
        chunks.push_back(DisplayChunk("\n", DisplayType::Text));
        if (expressions[i]->oper == Operand::If && expressions[i]->isRangeObject())
            gameState.incrementCurrentIndex();
    }*/
    return lines;
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