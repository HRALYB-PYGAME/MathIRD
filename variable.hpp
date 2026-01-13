#ifndef variable_hpp
#define variable_hpp

#include <string>
#include <variant>
#include <memory>
#include "variable_value.hpp"

class Node;

enum class Polarity{
    Normal,
    Inverted,
    Absolute,
    Neutral
};

struct ScoreParams{
    double knee;
    double offset;
    Polarity polarity;

    double getScore(double val);
};

ScoreParams getScoreParams(double knee, double offset, Polarity polarity);

class Variable {
    private:
        
    public:
        std::string name;
        ScoreParams scoreParams;
        std::unique_ptr<Node> unlockCondition;
        VariableValue defaultValue;
        Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, VariableValue defaultValue): 
            name(std::move(name)), scoreParams(scoreParams), unlockCondition(std::move(unlockCondition)), defaultValue(defaultValue) {};
};

#endif