#ifndef process_hpp
#define process_hpp

#include "expressiontree.hpp"
#include "insightable.hpp"
#include "term.hpp"

class GameState;

class Process : Insightable{
private:
    std::unique_ptr<Node> startCondition;
    std::unique_ptr<Node> endCondition;
    std::vector<std::unique_ptr<Term>> expressions;

    std::unique_ptr<Node> interval;

    std::string name;

public:
    std::string& getName() {return name;};
    double getInterval(GameState& gameState);

    std::vector<DisplayLine> insight(GameState& gameState, int level);
};

#endif