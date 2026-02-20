#ifndef game_state_hpp
#define game_state_hpp

#include "packet.hpp"
#include "process.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <map>

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
private:
    bool unlocked = false;

public:
    void unlock(){
        unlocked = true;
    };
    void lock(){
        unlocked = false;
    };
    bool isUnlocked() const{
        return unlocked;
    };
};

struct VariableEntry : Entry {
private:
    int blockCounter=0;

public:
    double value;
    double realValue;
    VariableEntry(double value);

    int getBlockedCounter() const{
        return blockCounter;
    };
    bool isBlocked() const{
        return blockCounter > 0;
    };
    void incrementBlockCounter(){
        blockCounter++;
    }
    void decrementBlockCounter(){
        blockCounter--;
    }
};

struct ButtonEntry : Entry {
    ButtonEntry();
};

struct ProcessEntry : Entry {
private:
    bool running = false;

public:
    bool isRunning() const{
        return running;
    };
    void start(){
        running = true;
    };
    void stop(){
        running = false;
    };
};

enum CalendarEntryType{
    ProcessTick
};

struct CalendarEntry{
    Process* process;
    CalendarEntryType type;
    double time;
};

class GameState{
    private:
        std::unordered_map<std::string, VariableEntry> variables;
        std::unordered_map<std::string, ButtonEntry> buttons;
        std::unordered_map<std::string, ProcessEntry> processes;

        // builtin variables
        uint64_t currentSeed;
        std::unordered_map<std::string, double> builtin;

        double inGameTime = 0;
        double forcedRandom = -1;

        std::unordered_map<std::string, std::vector<ConditionProbability>> probabilities;
        std::string currentTerm;
        size_t currentIndex;
        Insightable* currentInsightable = nullptr;

        std::vector<Packet> packets;
        std::vector<CalendarEntry> calendar;
        uint64_t seed = 0;
    public:
    
        std::vector<DisplayLine> currentInsight;
        GameState();
        GameState(const GameState& gs) = default;

        double getTotalScore() const;
        
        // Variables
        double getVarValue(std::string name);
        bool isVariableUnlocked(std::string name) const;
        void blockVariable(std::string name);
        void unblockVariable(std::string name);
        bool isVariableBlocked(std::string name) const;
        int getVariableBlockCounter(std::string name);
        void setVarValue(std::string name, double value);
        void addVarValue(std::string name, double value);
        void addVariables();
        void addVariable(Variable* variable);
        void updateVariables();
        void applyDeltas(VariableChanges changes);
        void applyDelta(std::string var, double delta);
        void applyNewValue(std::string var, double newValue);
        double getRealValue(std::string name) const;
        void setRealValue(std::string name, double value);
        void clearRealValues();
        void addBuiltin();
        
        // Buttons
        bool isButtonUnlocked(std::string name) const;
        void updateButtons();
        void addButtons();
        void addButton(Button* button);

        // Debug
        void printUnlocked();
        
        // Seed
        uint64_t getSeed() {return seed;};

        // Time
        double getInGameTime() {return inGameTime;};
        void addSecondsToInGameTime(double seconds) {inGameTime += seconds;};

        // Randomness
        double getCurrentrandom() const;
        void step();
        double insertNewConditionResult(bool result);
        void setCurrentTerm(std::string name) {currentTerm = name; currentIndex = 0;};
        void setCurrentIndex(size_t index) {currentIndex = index;};
        void incrementCurrentIndex() {currentIndex++;};

        void forceRandom(double val) { forcedRandom = val; };
        void freeRandom() { forcedRandom = -1; };

        // Packets
        std::vector<Packet>& getPackets() {return packets;};
        double getPacketsCount() {return (double)(packets.size());};
        double getPacketsCount(std::string var);
        void sendPacket(Packet packet, bool update);
        void sendPackets(std::vector<Packet>& packets);
        void updatePacketsAndRealValues();
        void addPacketFromAnExpression(const Expression& expression, std::vector<Packet>& packets, ButtonPosition startPos, SourceID source);
        std::vector<Packet> generatePackets(Button* button);
        std::vector<Packet> generatePackets(Process* process, ButtonPosition startPos);
        void addPacketFromAnExpression(const Expression& expression, std::vector<Packet>& packets, ButtonPosition startPos, double time, uint64_t& seed, SourceID source);
        std::vector<Packet> generatePackets(Button* button, ButtonPosition startPos, double time, uint64_t& seed);
        std::vector<Packet> generatePackets(Process* process, ButtonPosition startPos, double time, uint64_t& seed);

        // Calendar and processes
        bool isProcessUnlocked(std::string name) const;
        bool isProcessActive(std::string name) const;
        void addNewProcessEvent(Process* process, double seconds);
        std::vector<CalendarEntry>& getCalendar() {return calendar;};
        void updateProcesses();
        void addProcesss();
        void addProcess(Process* process);

        void updateEntries();
        void init();

        // Insight
        std::vector<DisplayLine>& getCurrentInsight() {return currentInsight;};
        void setCurrentInsight(std::vector<DisplayLine> insight);
        void setCurrentInsightable(Insightable* insightable);
        void updateCurrentInsight();
        std::map<SourceID, VariableChanges> predict(Button* button) const;
};



// void updateVariableSets(Term* term);

#endif