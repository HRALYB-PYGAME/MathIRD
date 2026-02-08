#ifndef expressiontree_hpp
#define expressiontree_hpp
#include "tokenizer.hpp"
#include "rangeobject.hpp"
#include <memory>
#include <set>

#define REDTXT  "\x1B[31m"
#define RESET   "\033[0m"
#define WHITEBG "\033[47m"
#define REVERSE "\033[30;47m"

class GameState;

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
    virtual std::unique_ptr<Node> clone() const = 0;
    
    virtual double evaluate(GameState& gameState, bool realValues=false) const = 0;
    virtual VariableChanges simulate(GameState& gameState) = 0;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    virtual std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) = 0; 

    virtual std::set<std::string> getDependencies() const = 0;
    virtual std::set<std::string> getInputs(bool root) const = 0;
    virtual std::set<std::string> getOutputs(bool root) const = 0;
    virtual std::set<std::string> getBlockers(bool root) const = 0;

    virtual bool isConstant(GameState& gameState) = 0;
    virtual bool isConstantValue(GameState& gameState, double val) = 0;
    virtual bool isRangeObject() = 0;
    virtual NodeType getType() = 0;

    virtual RangeObject getRangeObject() = 0;
    virtual std::unique_ptr<Node> getRandomDistribution() = 0;

    virtual std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const = 0;

    virtual std::string getVarString() const = 0;
};

struct ConstantNode : Node{
    double val;
    Operand oper = Operand::NoOperand;
    ConstantNode(double val): val(val) {};
    std::unique_ptr<Node> clone() const override;

    double evaluate([[maybe_unused]] GameState& gameState, bool realValues=false) const override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override { return {}; };
    std::set<std::string> getInputs([[maybe_unused]] bool root) const override { return {}; };
    std::set<std::string> getOutputs([[maybe_unused]] bool root) const override { return {}; };
    std::set<std::string> getBlockers(bool root) const override { return {}; };

    bool isConstant([[maybe_unused]] GameState& gameState) override {return true;};
    bool isConstantValue([[maybe_unused]] GameState& gameState, double val) override;
    virtual bool isRangeObject() override {return false;};
    NodeType getType() override { return NodeType::Constant;};

    RangeObject getRangeObject() override { return RangeObject(val); };
    std::unique_ptr<Node> getRandomDistribution() override;

    std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const override {
        if (base) return nullptr;
        return std::make_unique<ConstantNode>(val);
    };

    std::string getVarString() const override { return ""; }
};

struct GeneralConstantNode : ConstantNode{
    GeneralConstantNode() : ConstantNode(0) {};

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    NodeType getType() override { return NodeType::GeneralConstant; };
};

struct VariableNode : Node{
    std::string var;
    Operand oper = Operand::NoOperand;
    VariableFlags flags;

    VariableNode(std::string var, VariableFlags flags): var(var), flags(flags) {};
    std::unique_ptr<Node> clone() const override;

    double evaluate(GameState& gameState, bool realValues=false) const override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override;
    std::set<std::string> getInputs(bool root) const override;
    std::set<std::string> getOutputs(bool root) const override;
    std::set<std::string> getBlockers(bool root) const override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    virtual bool isRangeObject() override {if (var == "_R" or var == "_NR") return true; return false;};
    NodeType getType() override { return NodeType::Variable;};

    RangeObject getRangeObject() override { if (var == "_R" or var == "_NR") return RangeObject("_R", VariableFlags()); return RangeObject(var, flags); };
    std::unique_ptr<Node> getRandomDistribution() override;

    bool isSoft() const {return flags.soft;};
    bool isFluid() const {return flags.fluid;};
    bool isConstant() const {return flags.constant;};
    bool isReal() const {return flags.real;};

    std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const override;

    std::string getVarString() const override { return var; };
};

struct OperandNode : public Node{
    Operand oper;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    OperandNode(Operand oper, std::unique_ptr<Node> left, std::unique_ptr<Node> right): oper(oper), left(std::move(left)), right(std::move(right)) {};
    std::unique_ptr<Node> clone() const override;

    double evaluate(GameState& gameState, bool realValues=false) const override;
    VariableChanges simulate(GameState& gameState) override;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override;
    std::set<std::string> getInputs(bool root) const override;
    std::set<std::string> getOutputs([[maybe_unused]] bool root) const override;
    std::set<std::string> getBlockers(bool root) const override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    virtual bool isRangeObject() override {if (left->isRangeObject() || right->isRangeObject()) return true; return false;};
    NodeType getType() override { return NodeType::Operand;};

    RangeObject getRangeObject() override;
    std::unique_ptr<Node> getRandomDistribution() override;

    std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const override;

    std::string getVarString() const override { return ""; };
};

struct Expression{
    std::unique_ptr<Node> expr;
    std::set<std::string> variableLocks;
    Expression clone() const;

    Expression(std::unique_ptr<Node> expr, std::set<std::string> variableLocks): expr(std::move(expr)), variableLocks(variableLocks) {};

    double evaluate(GameState& gameState, bool realValues = false){ return expr->evaluate(gameState, realValues); };

    std::set<std::string>& getVariableLocks() {return variableLocks;};
};

Expression construct(std::vector<Token> tokens);

#endif