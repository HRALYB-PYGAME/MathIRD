#ifndef NDEBUG
    #define LOG(msg) std::cout << "[DEBUG] " << msg << "\n"
#else
    #define LOG(msg)  // Does nothing in Release
#endif

#ifndef utils_hpp
#define utils_hpp
#include <string>
#include <unordered_map>
#include <math.h>
#include <chrono>
#include "raylib.h"

#define RANDOM_MAX (double)0xFFFFFFFFFFFFFFFFULL
#define PACKET_WEIGHT_MAX 10*M_PI*M_PI
#define PACKET_SPEED 1

class Variable;
class GameState;
class Button;
class Process;

std::string formatDouble(double val, bool includeSign=false);

class Defs {
public:
    static std::unordered_map<std::string, Variable> vars;
    static std::unordered_map<std::string, Button>   btns;
    static std::unordered_map<std::string, Process>  procs;

    static void addVariable(Variable var);
    static Variable* getVariable(std::string name);

    static void addButton(Button btn);
    static Button* getButton(std::string name);

    static void addProcess(Process proc);
    static Process* getProcess(std::string name);

    static void loadVariables(std::string path, std::unordered_map<std::string, std::string>&);
    static void loadButtons(std::string path);
    static void loadProcesses(std::string path);

    static void linkVariableHomeButtons(std::unordered_map<std::string, std::string>&);
    // Load()
};

struct ButtonPosition{
    int row;
    int col;

    bool isSameAs(ButtonPosition other){
        return other.row == row && other.col == col;
    }
};

struct VariableFlags{
    bool soft = false; //~var - can bypass variable not being unlocked
    bool fluid = false; //var~ - can bypass lock counter
    bool constant = false; //$var - take value of the variable before sending into packet
    bool real = false; //var? - value of the variable after all packets arrive

    // ~varName
    void setSoft() {soft = true;};
    // varName~
    void setFluid() {fluid = true;};
    // $varName
    void setConstant() {constant = true;};
    // varName?
    void setReal() {real = true;};
};

double getDistance(ButtonPosition start, ButtonPosition end);

inline std::chrono::steady_clock::duration secondsToDuration(double seconds) {
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(seconds)
    );
}

inline double durationToSeconds(std::chrono::steady_clock::duration duration) {
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();;
}

inline double getRandom(uint64_t& currentSeed){
    uint64_t multiplier = 6364136223846793005ULL;
    uint64_t increment = 1442695040888963407ULL;
    currentSeed = currentSeed*multiplier + increment;
    return currentSeed;
}

#endif