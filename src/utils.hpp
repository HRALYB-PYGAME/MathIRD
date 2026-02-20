#define NDEBUG

#ifndef NDEBUG
    #define LOG(msg) std::cout << "[DEBUG] " << msg << "\n" << std::flush
#else
    #define LOG(msg)  // Does nothing in Release
#endif

#ifndef utils_hpp
#define utils_hpp
#include <string>
#include <unordered_map>
#include <math.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include "raylib.h"

#define RANDOM_MAX (double)0xFFFFFFFFFFFFFFFFULL
#define PACKET_WEIGHT_MAX 10*M_PI*M_PI
#define PACKET_SPEED 1

class Variable;
class Button;
class Process;
class Insightable;

inline namespace Log {
    static std::string currentFile = "Unknown";
    const int typeWidth = 10;
    const int pathWidth = 40;

    void setContext(const std::string& path) {
        currentFile = path;
    };
    
    void error(const std::string& message);

    void warn(const std::string& message);

    void info(const std::string& message);
}

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

    static Insightable* getTarget(std::string targetPath);

    static void linkVariableHomeButtons(std::unordered_map<std::string, std::string>&);
    static void bind();
    // Load()
};

struct ButtonPosition{
    int row;
    int col;

    bool isSameAs(ButtonPosition other){
        return other.row == row && other.col == col;
    }

    ButtonPosition(): row(0), col(0) {};
    ButtonPosition(int row, int col): row(row), col(col) {};
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

using Clock = std::chrono::steady_clock;

double getDistance(ButtonPosition start, ButtonPosition end);

inline Clock::duration secondsToDuration(double seconds) {
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(seconds)
    );
}

inline double durationToSeconds(Clock::duration duration) {
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();;
}

inline double getRandom(uint64_t& currentSeed){
    uint64_t multiplier = 6364136223846793005ULL;
    uint64_t increment = 1442695040888963407ULL;
    currentSeed = currentSeed*multiplier + increment;
    return currentSeed;
}

#endif