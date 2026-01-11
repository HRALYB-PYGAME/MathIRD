#include "game_state.hpp"
#include "expressiontree.hpp"
#include <iostream>

GameState::GameState(){
}

double GameState::getScore(){
    double score = 0;
    for (auto& [name, var] : this->variables){
        score += var.getScore();
    }
    return score;
}

Variable* GameState::getVar(std::string name){
    auto pair = this->variables.find(name); 
    if (pair != this->variables.end()){
        return &(pair->second);
    }
    else{
        return nullptr;
    }
}

void GameState::applyChanges(VariableChanges changes){
    for (auto& [var, val] : changes.changes){
        this->variables.at(var).add(val);
    }
    this->updateVariables();
}

void GameState::updateVariables(){
    for (auto& [name, cond] : this->unlockConditions){
        if (!isVariableUnlocked(name) && cond->evaluate(*this).getAsBool()){
            this->variables.at(name).unlock();
        }
    }
}

bool GameState::isVariableUnlocked(std::string name){
    if (name[0] == '_') return true;
    if (this->variables.find(name) == this->variables.end())
        return false;
    return this->variables.at(name).isUnlocked();
}

void GameState::addVariable(std::string name, Variable var, std::shared_ptr<Node> condition){
    this->variables.emplace(name, var);
    this->unlockConditions[name] = condition;
}

void GameState::printUnlocked(){
    for (auto& [name, var] : this->variables){
        if (var.isUnlocked())
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
    if (name == "_R")
        return VariableValue(currentSeed/RANDOM_MAX);
    if (name == "_NR"){
        step();
        return VariableValue(currentSeed/RANDOM_MAX);
    }
    if (this->variables.find(name) != this->variables.end())
        return variables.at(name).getValue();
    return VariableValue(0.f);
}

double GameState::getVarValueAsDouble(std::string name){
    if (name == "_R")
        return currentSeed/RANDOM_MAX;
    if (name == "_NR"){
        step();
        return currentSeed/RANDOM_MAX;
    }
    if (this->variables.find(name) != this->variables.end())
        return variables.at(name).getValue().getAsDouble();
    return 0.0;
}
