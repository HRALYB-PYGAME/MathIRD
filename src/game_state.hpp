#ifndef game_state_hpp
#define game_state_hpp
#include <unordered_map>
#include "variable_value.hpp"
#include "variable.hpp"
#include <vector>
#include <string>
#include "insightable.hpp"

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

struct VariableEntry {
    VariableValue value;
    bool isUnlocked;

    void unlock(){
        isUnlocked = true;
    };

    void lock(){
        isUnlocked = false;
    };
};

class GameState{
    private:
        // std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, VariableEntry> variables;

        uint64_t currentSeed;
        double forcedRandom = -1;

        std::unordered_map<std::string, std::vector<ConditionProbability>> probabilities;
        std::string currentTerm;
        size_t currentIndex;
    public:
        std::vector<DisplayLine> currentInsight;
        GameState();
        double getTotalScore();

        // Variables
        //Variable* getVar(std::string name);
        VariableValue getVarValue(std::string name);
        double getVarValueAsDouble(std::string name);
        bool isVariableUnlocked(std::string name);
        void setVarValue(std::string name, VariableValue value);
        void addVarValue(std::string name, VariableValue value);
        void addVariable(Variable* variable);
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

        void forceRandom(double val) { forcedRandom = val; };
        void freeRandom() { forcedRandom = -1; };

        void setCurrentInsight(std::vector<DisplayLine> insight) {
            //if (insight.size() > 0)
                this->currentInsight = std::move(insight);
        };

        void updateVariableSets(Term* term);
};

#endif