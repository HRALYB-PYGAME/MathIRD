#ifndef variable_value_hpp
#define variable_value_hpp
#include <variant>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "insightable.hpp"

class GameState;

struct Range{
    double min;
    double max;
    double rand;

    Range(double min, double max, double rand): min(min), max(max), rand(rand) {};
};

class VariableChanges : Insightable{
public:
    std::unordered_map<std::string, Range> changes;

    VariableChanges(): changes({}) {};
    VariableChanges add(const VariableChanges& varc);
    VariableChanges add(std::string var, double val);
    VariableChanges add(std::string var, double min, double max, double rand);
    
    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
};

#endif