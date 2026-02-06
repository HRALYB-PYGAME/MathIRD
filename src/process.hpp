#ifndef process_hpp
#define process_hpp

#include "term.hpp"

class GameState;

class Process : Insightable{
private:
    std::unique_ptr<Node> startCondition;
    std::unique_ptr<Node> endCondition;
    std::vector<std::unique_ptr<Term>> terms;

    std::unique_ptr<Node> interval;

    std::string name;

public:
    Process(std::string name): name(name) {};

    std::string& getName() {return name;};
    double getInterval(GameState& gameState);

    bool isStartConditionMet(GameState& gameState);
    bool isEndConditionMet(GameState& gameState);

    void setStartCondition(std::unique_ptr<Node> startCondition);
    void setEndCondition(std::unique_ptr<Node> endCondition);
    void setInterval(std::unique_ptr<Node> interval);

    void addTerm(std::unique_ptr<Term>);
    const std::vector<std::unique_ptr<Term>>& getTerms() {return terms;};
    const std::vector<Expression> getExpressions(GameState& gameState) const;

    std::vector<DisplayLine> insight(GameState& gameState, int level);
};

#endif