#include "term.hpp"

bool Term::isConditionMet(GameState& gameState){
    return this->condition->evaluate(gameState).getAsBool();
}

VariableChanges Term::simulate(GameState& gameState){
    VariableChanges changes;
    if (!this->condition->evaluate(gameState).getAsBool()){
        LOG("term.cpp\tsimulate() of TERM " << name << " IS NOT UNLOCKED");
        return changes;
    }
    for(size_t i=0; i<this->expressions.size(); i++){
        changes.add(this->expressions[i]->simulate(gameState));
    }
    return changes;
}

/// @brief Returns the insight of a term. Can differ depending on the GameState provided.
/// @param gameState reference to current GameState
/// @param level level of the insight where 0 is root
/// @return vector of DisplayLines, ready to be printed on the screen
std::vector<DisplayLine> Term::insight(GameState& gameState, int level){
    gameState.setCurrentTerm(this->name);
    std::vector<DisplayLine> lines;

    DisplayLine line;
    if (isConditionMet(gameState)) line.appendTextChunk("TERM IS UNLOCKED");
    else line.appendTextChunk("TERM IS LOCKED");
    lines.push_back(line);

    auto simulationResultInsight = simulate(gameState).insight(gameState, 0);
    lines.insert(lines.end(), simulationResultInsight.begin(), simulationResultInsight.end());

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

/// @brief Changes term's condition.
/// @param condition unique_ptr to a Node
void Term::setCondition(std::unique_ptr<Node> condition) {
    this->condition = std::move(condition);
}

/// @brief Expands term's behavior by adding a new expression.
/// @param expression unique_ptr to a Node
void Term::addExpression(std::unique_ptr<Node> expression) {
    this->expressions.insert(this->expressions.end(), std::move(expression));
}

/// @brief Updates set of dependencies, inputs and outputs. These updates will reflect into variables.
void Term::updateSets(){
    updateDependencies();
    updateInputs();
    updateOutputs();

    for(auto& name : getDependencies()){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr)
            var->addTermAsDependency(this);
    }
    for(auto& name : getInputs()){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr)
            var->addTermAsInput(this);
    }
    for(auto& name : getOutputs()){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr)
            var->addTermAsOutput(this);
    }
}

/// @brief Updates term's dependencies set. Dependencies are variables that needs to be unlocked in order for term to be unlocked as well.
void Term::updateDependencies(){
    dependencies.clear();
    auto condDeps = condition->getDependencies();
    dependencies.insert(condDeps.begin(), condDeps.end());
    for (size_t i=0; i<expressions.size(); i++){
        auto exprDeps = expressions[i]->getDependencies();
        dependencies.insert(exprDeps.begin(), exprDeps.end());
    }
}

/// @brief Updates term's inputs set. Inputs are variables that can change behavior of such term.
void Term::updateInputs(){
    inputs.clear();
    for (size_t i=0; i<expressions.size(); i++){
        auto exprIns = expressions[i]->getInputs(true);
        inputs.insert(exprIns.begin(), exprIns.end());
    }
}

/// @brief Updates term's outputs set. Outputs are variables than can be changed by the term's execution.
void Term::updateOutputs(){
    outputs.clear();
    for (size_t i=0; i<expressions.size(); i++){
        auto exprOuts = expressions[i]->getOutputs(true);
        outputs.insert(exprOuts.begin(), exprOuts.end());
    }
}

/// @brief Checks if all dependent variables are unlocked.
/// @param gameState reference to current GameState
/// @return true when term is unlocked, false otherwise
bool Term::isUnlocked(GameState& gameState){
    for (const std::string& var : dependencies) {
        if (!gameState.isVariableUnlocked(var)){
            LOG("term.cpp\tisUnlocked() TERM " << name << " is locked because " << var << " is.");
            return false;
        }
    }
    return true;
}