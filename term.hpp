#ifndef term_hpp
#define term_hpp
#include "expressiontree.hpp"
#include <vector>

class Term{
private:
    std::unique_ptr<Node> condition;
    std::vector<std::unique_ptr<Node>> expressions;
public:
    void setCondition(std::unique_ptr<Node> condition);
    void addExpression(std::unique_ptr<Node> expression);
    bool isConditionMet(GameState& gameState);
    VariableChanges simulate(GameState& gameState);
};

#endif