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

#define PACKET_SPEED 1

class Variable;
class GameState;
class Button;

std::string formatDouble(double val, bool includeSign=false);

class Defs {
public:
    static std::unordered_map<std::string, Variable> vars;
    static std::unordered_map<std::string, Button>   btns;

    static void addVariable(Variable var);
    static Variable* getVariable(std::string name);

    static void addButton(Button btn);
    static Button* getButton(std::string name);

    static void loadVariables(std::string path, std::unordered_map<std::string, std::string>&);
    static void loadButtons(std::string path);

    static void linkVariableHomeButtons(std::unordered_map<std::string, std::string>&);
    // Load()
};

struct ButtonPosition{
    int row;
    int col;
};

double getDistance(ButtonPosition start, ButtonPosition end);

inline std::chrono::steady_clock::duration to_duration(double seconds) {
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(seconds)
    );
}

#endif