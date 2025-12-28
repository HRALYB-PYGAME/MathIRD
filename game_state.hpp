#ifndef game_state_hpp
#define game_state_hpp
#include <unordered_map>
#include "variable.hpp"
#include <string>

class GameState{
    private:
        std::unordered_map<std::string, Variable> variables;
    public:
        GameState();
        double getScore();
        Variable* getVar(std::string name);
};

#endif