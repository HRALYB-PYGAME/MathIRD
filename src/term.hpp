#ifndef term_hpp
#define term_hpp
//#include "expressiontree.hpp"
#include "variable.hpp"
#include <vector>
#include <set>

class Button;
class GameState;
class Packet;

class Term : public Insightable{
private:
    std::unique_ptr<Node> condition;
    std::vector<Expression> expressions;
    std::set<std::string> dependencies;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::set<std::string> blockers;
    std::string name;
    Button* buttonParent;
    Process* processParent;
public:
    Term(std::string name): name(name) {};

    void setCondition(std::unique_ptr<Node> condition);
    void addExpression(Expression expression);
    VariableChanges simulate(GameState& gameState);
    void updateDependencies();
    void updateInputs();
    void updateOutputs();
    void updateBlockers();
    void updateSets();
    void printSets();
    bool isLocked(GameState& gameState) const override;
    bool isActive(GameState& gameState) const override;
    bool isBlocked(GameState& gameState) const override;
    std::vector<DisplayLine> insight(GameState& gameState, int level) override;

    std::set<std::string>& getDependencies() {return dependencies;};
    std::set<std::string> getInputs(bool root, std::string function="") const override;
    std::set<std::string>& getOutputs()      {return outputs;};
    std::set<std::string>& getBlockers()     {return blockers;};
    const std::vector<Expression>& getExpressions() const;
    std::string getName() {return name;};
    Button& getParentButton() {return *buttonParent;};
    Process& getParentProcess() {return *processParent;};
    void setParent(Button& ref) {buttonParent = &ref;};
    void setParent(Process& ref) {processParent = &ref;};

    void bind();
};

#endif