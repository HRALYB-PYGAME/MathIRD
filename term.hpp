#ifndef term_hpp
#define term_hpp
#include "expressiontree.hpp"
#include <vector>
#include <set>

class Term{
private:
    std::unique_ptr<Node> condition;
    std::vector<std::unique_ptr<Node>> expressions;
    std::set<std::string> dependencies;
public:
    void setCondition(std::unique_ptr<Node> condition);
    void addExpression(std::unique_ptr<Node> expression);
    bool isConditionMet(GameState& gameState);
    VariableChanges simulate(GameState& gameState);
    void updateDependencies();
    bool isUnlocked(GameState& gameState);
    std::set<std::string> getDependencies() {return dependencies;};
};

#endif