#ifndef game_state_hpp
#define game_state_hpp
#include <unordered_map>
#include "variable_value.hpp"
#include "variable.hpp"
#include <vector>
#include <string>
#include "insightable.hpp"

class Node;
struct Packet;

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

struct Entry {
    bool unlocked = false;
    void unlock(){
        unlocked = true;
    };
    void lock(){
        unlocked = false;
    };
};

struct VariableEntry : Entry {
    VariableValue value;
    int version=0;
    int blockCounter=0;
    VariableEntry(VariableValue value);

    void incrementVersion() {
        LOG("game_state.hpp\tincrementVersion() newVersion=" << version+1);
        version++;
    };
};

struct ButtonEntry : Entry {
    ButtonEntry();
};

class GameState{
    private:
        std::unordered_map<std::string, VariableEntry> variables;
        std::unordered_map<std::string, ButtonEntry> buttons;

        uint64_t currentSeed;
        double forcedRandom = -1;

        std::unordered_map<std::string, std::vector<ConditionProbability>> probabilities;
        std::string currentTerm;
        size_t currentIndex;
        Insightable* currentInsightable = nullptr;

        std::vector<Packet> packets;
    public:

        std::vector<DisplayLine> currentInsight;
        GameState();
        double getTotalScore();

        // Variables
        int getVarVersion(std::string name);
        VariableValue getVarValue(std::string name);
        double getVarValueAsDouble(std::string name);
        bool isVariableUnlocked(std::string name);
        void blockVariable(std::string name);
        void unblockVariable(std::string name);
        bool isVariableBlocked(std::string name);
        void setVarValue(std::string name, VariableValue value);
        void addVarValue(std::string name, VariableValue value);
        void addVariables();
        void addVariable(Variable* variable);
        void updateVariables();
        void applyDeltas(VariableChanges changes);
        void applyDelta(std::string var, double delta);
        void applyNewValue(std::string var, double newValue);

        // Buttons
        bool isButtonUnlocked(std::string name);
        void updateButtons();
        void addButtons();
        void addButton(Button* button);

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

        // Packets
        std::vector<Packet>& getPackets() {return packets;};
        void addPacket(Packet packet);
        void addPackets(std::vector<Packet>& packets);
        void updatePackets();

        // Insight
        std::vector<DisplayLine>& getCurrentInsight() {return currentInsight;};

        void setCurrentInsight(std::vector<DisplayLine> insight);

        void setCurrentInsightable(Insightable* insightable);

        void updateCurrentInsight();
};

// void updateVariableSets(Term* term);

#endif