#ifndef game_state_hpp
#define game_state_hpp
#include <unordered_map>
#include "variable.hpp"
#include <vector>
#include <string>

#define RANDOM_MAX (double)0xFFFFFFFFFFFFFFFFULL

class Node;

struct ConditionProbability{
    double currentProbability = 0;
    int weight = 100;

    double update(bool result) {
        currentProbability *= weight;
        currentProbability += result;
        currentProbability /= (weight+1);
        return currentProbability;
    };
};

class GameState{
    private:
        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, std::shared_ptr<Node>> unlockConditions;
        uint64_t currentSeed;

        std::unordered_map<std::string, std::vector<ConditionProbability>> probabilities;
        std::string currentTerm;
        size_t currentIndex;
    public:
        GameState();
        double getScore();

        // Variables
        Variable* getVar(std::string name);
        VariableValue getVarValue(std::string name);
        double getVarValueAsDouble(std::string name);
        bool isVariableUnlocked(std::string name);
        void addVariable(std::string name, Variable var, std::shared_ptr<Node> condition);
        void updateVariables();
        void applyChanges(VariableChanges changes);

        // Debug
        void printUnlocked();

        // Randomness
        double getCurrentrandom();
        void step();
        double insertNewConditionResult(bool result);
        void setCurrentTerm(std::string name) {currentTerm = name; currentIndex = 0;};
        void setCurrentIndex(size_t index) {currentIndex = index;};
        void incrementCurrentIndex() {currentIndex++;};
};

#endif