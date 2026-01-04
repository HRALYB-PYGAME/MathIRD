#ifndef game_state_hpp
#define game_state_hpp
#include <unordered_map>
#include "variable.hpp"
#include <string>

class Node;

class GameState{
    private:
        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, std::shared_ptr<Node>> unlockConditions;
    public:
        GameState();
        double getScore();
        Variable* getVar(std::string name);
        void applyChanges(VariableChanges changes);
        void updateVariables();
        bool isVariableUnlocked(std::string name);
        void addVariable(std::string name, Variable var, std::shared_ptr<Node> condition);
        void printUnlocked();
};

#endif