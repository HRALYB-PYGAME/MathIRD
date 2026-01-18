#ifndef variable_hpp
#define variable_hpp

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include "variable_value.hpp"
#include "insightable.hpp"
#include <set>

class GameState;
class Node;
class Term;
class Button;

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
        std::string name;
        ScoreParams scoreParams;
        VariableValue defaultValue;
    public:
        std::unique_ptr<Node> unlockCondition;
        std::set<Term*> asDependencyInTerms;
        std::set<Term*> asInputInTerms;
        std::set<Term*> asOutputInTerms;
        std::set<Button*> displayedAtButtons;
        Button* homeButton;
        Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, VariableValue defaultValue);

        std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
        void addTermAsDependency(Term* term);
        void addTermAsInput(Term* term);
        void addTermAsOutput(Term* term);
        void addButtonAsDisplay(Button* button);
        void setHomeButton(Button* button);

        std::string getName() {return name;};
        ScoreParams getScoreParams() {return scoreParams;};
        VariableValue getDefaultValue() {return defaultValue;};

        void printDependencies();
};

#endif