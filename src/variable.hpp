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
        std::unique_ptr<Node> unlockCondition;
        double defaultValue;
        double granularity = 1;
        Button* homeButton;

        std::set<Term*> asDependencyInTerms;
        std::set<Term*> asInputInTerms;
        std::set<Term*> asOutputInTerms;
        std::set<Term*> asBlockerInTerms;
    public:
        std::set<Button*> displayedAtButtons;
        
        Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, double defaultValue);

        std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
        void addTermAsDependency(Term* term);
        void addTermAsInput(Term* term);
        void addTermAsOutput(Term* term);
        void addTermAsBlocker(Term* term);
        void addButtonAsDisplay(Button* button);

        std::string getName() {return name;};
        ScoreParams getScoreParams() {return scoreParams;};
        double getDefaultValue() {return defaultValue;};
        Button* getHomeButton() {return homeButton;};
        void setHomeButton(Button& ref) {homeButton = &ref;};
        Node& getUnlockCondition() {return *unlockCondition;};

        void setGranularity(double value) {granularity=value;};
        double getGranularity() {return granularity;};

        void printDependencies();
};

#endif