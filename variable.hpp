#ifndef variable_hpp
#define variable_hpp

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include "variable_value.hpp"
#include "insightable.hpp"

class GameState;

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

class Variable : public Insightable{
    private:
        
    public:
        std::string name;
        ScoreParams scoreParams;
        std::unique_ptr<Node> unlockCondition;
        VariableValue defaultValue;
        Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, VariableValue defaultValue);

        std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
};

#endif