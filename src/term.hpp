#ifndef term_hpp
#define term_hpp
//#include "expressiontree.hpp"
#include "insightable.hpp"
#include <vector>
#include <set>
#include "variable_value.hpp"

class Button;
struct Expression;

class Term : public Insightable{
private:
    std::unique_ptr<Node> condition;
    std::vector<Expression> expressions;
    std::set<std::string> dependencies;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::set<std::string> blockers;
    std::string name;
    Button* parent;
public:
    Term(std::string name): name(name) {};

    void setCondition(std::unique_ptr<Node> condition);
    void addExpression(Expression expression);
    bool isActive(GameState& gameState);
    VariableChanges simulate(GameState& gameState);
    void updateDependencies();
    void updateInputs();
    void updateOutputs();
    void updateBlockers();
    void updateSets();
    void printSets();
    bool isUnlocked(GameState& gameState);
    bool isUnblocked(GameState& gameState);
    std::vector<DisplayLine> insight(GameState& gameState, int level) override;

    std::set<std::string>& getDependencies() {return dependencies;};
    std::set<std::string>& getInputs()       {return inputs;};
    std::set<std::string>& getOutputs()      {return outputs;};
    std::set<std::string>& getBlockers()     {return blockers;};
    const std::vector<Expression>& getExpressions() const;
    std::string getName() {return name;};
    Button& getParent() {return *parent;};
    void setParent(Button& ref) {parent = &ref;};
};

#endif