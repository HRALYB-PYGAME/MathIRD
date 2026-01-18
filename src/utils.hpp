#ifndef utils_hpp
#define utils_hpp
#include <string>
#include <unordered_map>

class Variable;
class GameState;

std::string formatDouble(double val, bool includeSign=false);

class Defs {
public:
    static std::unordered_map<std::string, Variable> vars;

    static void addVariable(std::string name, Variable var);
    static Variable* getVariable(std::string name);
    // static void Load(); // Call once at start
};

#endif