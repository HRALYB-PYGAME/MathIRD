#include "game_state.hpp"
#include "expressiontree.hpp"
#include <iostream>
#include "variable.hpp"
#include "term.hpp"
#include "button.hpp"

GameState::GameState() : currentInsight() {
}

double GameState::getTotalScore(){
    double score = 0;
    for (auto& [name, varEntry] : variables){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr)
            score += var->getScoreParams().getScore(varEntry.value.getAsDouble());
    }
    return score;
}

void GameState::applyChanges(VariableChanges changes){
    for (auto& [var, val] : changes.changes){
        std::cout << "adding " << val.rand << " to " << var << "\n";
        addVarValue(var, VariableValue(val.rand));
        std::cout << "x val: " << getVarValueAsDouble("x") << "\n";
    }
    this->updateVariables();
}

void GameState::setVarValue(std::string name, VariableValue value){
    if (variables.find(name) != variables.end())
        variables.at(name).value.set(value);
}

void GameState::addVarValue(std::string name, VariableValue value){
    if (variables.find(name) != variables.end()){
        variables.at(name) = variables.at(name).value.add(value);
    }
    else
        std::cout << "not found\n";
}

void GameState::updateVariables(){
    for (auto& [name, entry] : variables){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr){
            if (!isVariableUnlocked(name) && var->getUnlockCondition().evaluate(*this).getAsBool()){
                entry.unlock();
            }
        }
    }
}

bool GameState::isVariableUnlocked(std::string name){
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).unlocked;
}

void GameState::addVariable(Variable* variable){
    VariableEntry entry(variable->getDefaultValue());
    std::string name = variable->getName();
    variables.insert({name, entry});
}

VariableEntry::VariableEntry(VariableValue value){
    this->value = value;
    lock();
}

ButtonEntry::ButtonEntry(){
    lock();
}

void GameState::addButton(Button* button){
    ButtonEntry entry;
    std::string name = button->getName();
    buttons.insert({name, entry});
}

bool GameState::isButtonUnlocked(std::string name){
    if (buttons.find(name) == buttons.end()) return false;
    return buttons.at(name).unlocked;
}

void GameState::updateButtons(){
    for (auto& [name, entry] : buttons){
        Button* btn = Defs::getButton(name);

        if (btn == nullptr) continue;
        
        if (!isButtonUnlocked(name) && btn->isUnlocked(*this))
            entry.unlock();
    }
}

void GameState::printUnlocked(){
    for (auto& [name, entry] : variables){
        if (entry.unlocked)
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
    if (this->variables.find(name) != this->variables.end()){
        return variables.at(name).value;
    }

    auto var = Defs::getVariable(name);
    if (var == nullptr)
        return VariableValue(0.f);
    addVariable(var);
    return getVarValue(name);
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
    if (this->variables.find(name) != this->variables.end()){
        return variables.at(name).value.getAsDouble();
    }
    
    auto var = Defs::getVariable(name);
    if (var == nullptr)
        return 0.0;
    addVariable(var);
    return getVarValueAsDouble(name);
}

void GameState::updateVariableSets(Term* term){
    for(auto& name : term->getDependencies()){
        if (variables.find(name) != variables.end()){
            Variable* var = Defs::getVariable(name);
            if (var != nullptr)
                var->addTermAsDependency(term);
        }
    }
    for(auto& name : term->getInputs()){
        if (variables.find(name) != variables.end()){
            Variable* var = Defs::getVariable(name);
            if (var != nullptr)
                var->addTermAsInput(term);
        }
    }
    for(auto& name : term->getOutputs()){
        if (variables.find(name) != variables.end()){
            Variable* var = Defs::getVariable(name);
            if (var != nullptr)
                var->addTermAsOutput(term);
        }
    }
}