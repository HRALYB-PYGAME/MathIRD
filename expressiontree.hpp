#ifndef expressiontree_hpp
#define expressiontree_hpp
#include "game_state.hpp"
#include "tokenizer.hpp"
#include <memory>

struct Node{
    virtual ~Node() = default;
    virtual VariableValue evaluate(GameState& gameState) = 0;
};

struct ConstantNode : public Node{
    VariableValue val;
    ConstantNode(VariableValue val): val(val) {};
    VariableValue evaluate(GameState& gameState) override;
};

struct VariableNode : public Node{
    std::string var;
    VariableNode(std::string var): var(var) {};
    VariableValue evaluate(GameState& gameState) override;
};

struct OperandNode : public Node{
    Operand oper;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    OperandNode(Operand oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right): oper(oper), left(std::move(left)), right(std::move(right)) {};
    VariableValue evaluate(GameState& gameState) override;
};

std::unique_ptr<Node> construct(std::vector<Token> tokens);

#endif