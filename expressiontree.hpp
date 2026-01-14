#ifndef expressiontree_hpp
#define expressiontree_hpp
#include "game_state.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"
#include "rangeobject.hpp"
#include "insightable.hpp"
#include <memory>
#include <set>

#define REDTXT  "\x1B[31m"
#define RESET   "\033[0m"
#define WHITEBG "\033[47m"
#define REVERSE "\033[30;47m"

enum class NodeType{
    Constant,
    GeneralConstant,
    Variable,
    Operand,
    Unknown
};

struct GeneralConstantNode;

struct Node : public Insightable{
    virtual ~Node() = default;
    Operand oper = Operand::NoOperand;
    virtual VariableValue evaluate(GameState& gameState) = 0;
    virtual VariableChanges simulate(GameState& gameState) = 0;

    //virtual std::string insight(GameState& gameState, int level) = 0;
    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    virtual std::string arithmeticalInsight() = 0; 

    virtual std::set<std::string> getDependencies() = 0;
    virtual std::set<std::string> getInputs(bool root) = 0;
    virtual std::set<std::string> getOutputs(bool root) = 0;

    virtual bool isConstant(GameState& gameState) = 0;
    virtual bool isConstantValue(GameState& gameState, double val) = 0;
    virtual bool isRangeObject() = 0;
    virtual NodeType getType() = 0;

    virtual RangeObject getRangeObject() = 0;
    virtual std::unique_ptr<Node> getRandomDistribution() = 0;
};

struct ConstantNode : public Node{
    VariableValue val;
    Operand oper = Operand::NoOperand;
    ConstantNode(VariableValue val): val(val) {};

    VariableValue evaluate([[maybe_unused]] GameState& gameState) override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::string arithmeticalInsight() override { return formatDouble(val.getAsDouble()); };

    std::set<std::string> getDependencies() override { std::set<std::string> deps; return deps; };
    std::set<std::string> getInputs([[maybe_unused]] bool root) override { std::set<std::string> deps; return deps; };
    std::set<std::string> getOutputs([[maybe_unused]] bool root) override { std::set<std::string> deps; return deps; };

    bool isConstant([[maybe_unused]] GameState& gameState) override {return true;};
    bool isConstantValue([[maybe_unused]] GameState& gameState, double val) override;
    virtual bool isRangeObject() override {return false;};
    NodeType getType() override { return NodeType::Constant;};

    RangeObject getRangeObject() override { return RangeObject(val); };
    std::unique_ptr<Node> getRandomDistribution() override;
};

struct GeneralConstantNode : ConstantNode{
    GeneralConstantNode() : ConstantNode(VariableValue(0.0)) {};

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
    std::string arithmeticalInsight() override { return "C"; };

    NodeType getType() override { return NodeType::GeneralConstant; };
};

struct VariableNode : public Node{
    std::string var;
    bool soft;
    Operand oper = Operand::NoOperand;
    VariableNode(std::string var, bool soft): var(var), soft(soft) {};

    VariableValue evaluate(GameState& gameState) override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::string arithmeticalInsight() override { if (var == "_R" || var == "_NR") return "R"; return var; };

    std::set<std::string> getDependencies() override;
    std::set<std::string> getInputs(bool root) override;
    std::set<std::string> getOutputs(bool root) override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    virtual bool isRangeObject() override {if (var == "_R" or var == "_NR") return true; return false;};
    NodeType getType() override { return NodeType::Variable;};

    RangeObject getRangeObject() override { if (var == "_R" or var == "_NR") return RangeObject("_R", false); return RangeObject(var, soft); };
    std::unique_ptr<Node> getRandomDistribution() override;
};

struct OperandNode : public Node{
    Operand oper;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    OperandNode(Operand oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right): oper(oper), left(std::move(left)), right(std::move(right)) {};

    VariableValue evaluate(GameState& gameState) override;
    VariableChanges simulate(GameState& gameState) override;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    std::string arithmeticalInsight() override;

    std::set<std::string> getDependencies() override;
    std::set<std::string> getInputs(bool root) override;
    std::set<std::string> getOutputs([[maybe_unused]] bool root) override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    virtual bool isRangeObject() override {if (left->isRangeObject() || right->isRangeObject()) return true; return false;};
    NodeType getType() override { return NodeType::Operand;};

    RangeObject getRangeObject() override;
    std::unique_ptr<Node> getRandomDistribution() override;
};

std::unique_ptr<Node> construct(std::vector<Token> tokens);

#endif