#ifndef utils_hpp
#define utils_hpp
#include <string>
#include <unordered_map>

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

    static void loadVariables(std::string path);
    // Load()
};

#endif