#ifndef expressiontree_hpp
#define expressiontree_hpp
#include "game_state.hpp"
#include "tokenizer.hpp"
#include <memory>

#define RED "\x1B[31m"
#define RESET "\033[0m"
#define WHITEBG "\033[47m"
#define REVERSE "\033[30;47m"

enum class NodeType{
    Constant,
    Variable,
    Operand,
    Unknown
};

std::string formatDouble(double val);

struct Node{
    virtual ~Node() = default;
    virtual VariableValue evaluate(GameState& gameState) = 0;
    virtual VariableChanges simulate(GameState& gameState) = 0;
    virtual std::string insight(GameState& gameState, int level) = 0;
    virtual bool isConstant(GameState& gameState) = 0;
    virtual NodeType getType() = 0;
};

struct ConstantNode : public Node{
    VariableValue val;
    ConstantNode(VariableValue val): val(val) {};
    VariableValue evaluate(GameState& gameState) override;
    VariableChanges simulate(GameState& gameState) override { return VariableChanges(); };
    std::string insight(GameState& gameState, int level) override;
    bool isConstant(GameState& gameState) override {return true;};
    NodeType getType() override { return NodeType::Constant;};
};

struct VariableNode : public Node{
    std::string var;
    bool soft;
    VariableNode(std::string var, bool soft): var(var), soft(soft) {};
    VariableValue evaluate(GameState& gameState) override;
    VariableChanges simulate(GameState& gameState) override { return VariableChanges(); };
    std::string insight(GameState& gameState, int level) override;
    bool isConstant(GameState& gameState) override;
    NodeType getType() override { return NodeType::Variable;};
};

struct OperandNode : public Node{
    Operand oper;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    OperandNode(Operand oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right): oper(oper), left(std::move(left)), right(std::move(right)) {};
    VariableValue evaluate(GameState& gameState) override;
    VariableChanges simulate(GameState& gameState) override;
    std::string insight(GameState& gameState, int level) override;
    bool isConstant(GameState& gameState) override;
    NodeType getType() override { return NodeType::Operand;};
};

std::unique_ptr<Node> construct(std::vector<Token> tokens);

#endif