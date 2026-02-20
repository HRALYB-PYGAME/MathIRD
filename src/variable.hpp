#ifndef variable_hpp
#define variable_hpp

#include "expressiontree.hpp"
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <set>

class GameState;
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

enum VariableType{
    Boolean, // 0, 1
    Double, // 0.5, -4,8, 2.3 f.e.
    Int, // ..., -2, -1, 0, 1, 2, ...
    Percentage, // 0-1
    Enum, // 0, 1, 2, ...
    Unkown
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
        VariableType type;
        std::vector<std::string> stateNames;

        std::set<Term*> asDependencyInTerms;
        std::set<Term*> asInputInTerms;
        std::set<Term*> asOutputInTerms;
        std::set<Term*> asBlockerInTerms;
    public:
        std::set<Button*> displayedAtButtons;
        
        Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, double defaultValue, VariableType type);

        std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
        std::set<std::string> getInputs(bool root, std::string function="") const override;
        void addTermAsDependency(Term* term);
        void addTermAsInput(Term* term);
        void addTermAsOutput(Term* term);
        void addTermAsBlocker(Term* term);
        void addButtonAsDisplay(Button* button);

        std::string getName() const {return name;};
        std::string getStateNameAt(size_t index) const {if (index < 0 || index >= stateNames.size()) return ""; return stateNames[index]; }; 
        void addState(std::string name) {stateNames.push_back(name);};
        size_t getStateCount() const {return stateNames.size();};
        bool doesStateExist(std::string name) const {
            for(auto& state : stateNames){
                if (state == name) return true;
            }
            return false;
        };
        VariableType getType() const {return type;};
        ScoreParams getScoreParams() const {return scoreParams;};
        double getDefaultValue() const {return defaultValue;};
        Button* getHomeButton() {return homeButton;};
        void setHomeButton(Button& ref) {homeButton = &ref;};
        Node& getUnlockCondition() {return *unlockCondition;};

        void setGranularity(double value) {granularity=value;};
        double getGranularity() const {return granularity;};

        double constrain(double value) const;
        void bind();

        void printDependencies();

        bool isLocked(GameState& gameState) const override;
        bool isActive(GameState& gameState) const override;
        bool isBlocked(GameState& gameState) const override;
};

#endif