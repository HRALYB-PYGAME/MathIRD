#include "game_state.hpp"
#include "expressiontree.hpp"
#include <iostream>

GameState::GameState() : currentInsight() {
}

double GameState::getTotalScore(){
    double score = 0;
    for (auto& [name, varEntry] : variables){
        score += varEntry.getScore();
    }
    return score;
}

Variable* GameState::getVar(std::string name){
    if (variables.find(name) == variables.end()) return nullptr;
    return variables.at(name).definition;
}

void GameState::applyChanges(VariableChanges changes){
    for (auto& [var, val] : changes.changes){
        addVarValue(var, VariableValue(val));
    }
    this->updateVariables();
}

void GameState::setVarValue(std::string name, VariableValue value){
    if (variables.find(name) != variables.end())
        variables.at(name).value.set(value);
}

void GameState::addVarValue(std::string name, VariableValue value){
    if (variables.find(name) != variables.end())
        variables.at(name).value.add(value);
}

void GameState::updateVariables(){
    for (auto& [name, entry] : variables){
        if (!isVariableUnlocked(name) && entry.definition->unlockCondition->evaluate(*this).getAsBool()){
            entry.unlock();
        }
    }
}

bool GameState::isVariableUnlocked(std::string name){
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).isUnlocked;
}

void GameState::addVariable(Variable* variable){
    VariableEntry entry = {variable, variable->defaultValue, false};
    std::string name = variable->name;
    variables.insert({name, entry});
}

void GameState::printUnlocked(){
    for (auto& [name, entry] : variables){
        if (entry.isUnlocked)
            std::cout << "unlocked: " << name << std::endl;
    }
}

double GameState::getCurrentrandom(){
    return currentSeed;
}

void GameState::step(){
    uint64_t multiplier = 6364136223846793005ULL;
    uint64_t increment = 1442695040888963407ULL;
    currentSeed = currentSeed*multiplier + increment;
}

double GameState::insertNewConditionResult(bool result){
    auto& vec = probabilities[currentTerm];
    if (vec.size() >= currentIndex){
        vec.resize(currentIndex+1);
    }
    return vec[currentIndex].update(result);
}

VariableValue GameState::getVarValue(std::string name){
    if (name == "_R"){
        if (forcedRandom != -1) return VariableValue(forcedRandom);
        return VariableValue(currentSeed/RANDOM_MAX);
    }
    if (name == "_NR"){
        step();
        if (forcedRandom != -1) return VariableValue(forcedRandom);
        return VariableValue(currentSeed/RANDOM_MAX);
    }
    if (this->variables.find(name) != this->variables.end())
        return variables.at(name).value;
    return VariableValue(0.f);
}

double GameState::getVarValueAsDouble(std::string name){
    if (name == "_R"){
        if (forcedRandom != -1) return forcedRandom; 
        return currentSeed/RANDOM_MAX;
    }
    if (name == "_NR"){
        step();
        if (forcedRandom != -1) return forcedRandom;
        return currentSeed/RANDOM_MAX;
    }
    if (this->variables.find(name) != this->variables.end())
        return variables.at(name).value.getAsDouble();
    return 0.0;
}
