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
    std::set<std::string> inputs;
    std::string name;
public:
    Term(std::string name): name(name) {};

    void setCondition(std::unique_ptr<Node> condition);
    void addExpression(std::unique_ptr<Node> expression);
    bool isConditionMet(GameState& gameState);
    VariableChanges simulate(GameState& gameState);
    void updateDependencies();
    void updateInputs();
    bool isUnlocked(GameState& gameState);
    std::string insight(GameState& gameState);

    std::set<std::string> getDependencies() {return dependencies;};
    std::set<std::string> getInputs()       {return inputs;};
};

#endif