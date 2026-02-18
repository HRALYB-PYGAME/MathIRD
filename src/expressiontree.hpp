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
    Function,
    Unknown
};

enum class ValueType{
    Current,
    Real,
    Default
};

struct GeneralConstantNode;

struct Node : public Insightable{
    virtual ~Node() = default;
    virtual std::unique_ptr<Node> clone() const = 0;
    
    virtual double evaluate(GameState& gameState, ValueType valType = ValueType::Current) const= 0;
    virtual double getProgress(GameState& gameState, int& weight) const= 0;
    virtual VariableChanges simulate(GameState& gameState) = 0 ;
    virtual Node* normalize(bool negate = false) = 0;
    virtual void bind() = 0;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override = 0;
    virtual std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) = 0; 

    virtual std::set<std::string> getDependencies() const = 0;
    // std::set<std::string> getInputs(bool root) const override { };
    virtual std::set<std::string> getOutputs(bool root) const = 0;
    virtual std::set<std::string> getBlockers(bool root) const = 0;

    virtual bool isConstant(GameState& gameState) = 0;
    virtual bool isConstantValue(GameState& gameState, double val) = 0;
    virtual bool isRangeObject() = 0;
    virtual bool containEnumLikeVariable() const = 0;
    virtual NodeType getType() = 0;

    virtual RangeObject getRangeObject() = 0;
    virtual std::unique_ptr<Node> getRandomDistribution() = 0 ;

    virtual std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const = 0;

    virtual std::string getVarString() const = 0;
};

struct ConstantNode : Node{
    double val;
    Operand oper = Operand::NoOperand;
    ConstantNode(double val): val(val) {};
    std::unique_ptr<Node> clone() const override;

    double evaluate([[maybe_unused]] GameState& gameState, ValueType valType = ValueType::Current) const override;
    double getProgress(GameState& gameState, int& weight) const override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };
    Node* normalize(bool negate = false) override;
    void bind() override {};

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override { return {}; };
    std::set<std::string> getInputs([[maybe_unused]] bool root, [[maybe_unused]] std::string function = "") const override { return {}; };
    std::set<std::string> getOutputs([[maybe_unused]] bool root) const override { return {}; };
    std::set<std::string> getBlockers(bool root) const override { return {}; };

    bool isConstant([[maybe_unused]] GameState& gameState) override {return true;};
    bool isConstantValue([[maybe_unused]] GameState& gameState, double val) override;
    bool isRangeObject() override {return false;};
    bool containEnumLikeVariable() const override {return false;};
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

    double evaluate(GameState& gameState, ValueType valType = ValueType::Current) const override;
    double getProgress(GameState& gameState, int& weight) const override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };
    Node* normalize(bool negate = false) override;
    void bind() override {};

    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override;
    std::set<std::string> getInputs(bool root, [[maybe_unused]] std::string function = "") const override;
    std::set<std::string> getOutputs(bool root) const override;
    std::set<std::string> getBlockers(bool root) const override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    bool isRangeObject() override {if (var == "_R" or var == "_NR") return true; return false;};
    bool containEnumLikeVariable() const override;
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

    double evaluate(GameState& gameState, ValueType valType = ValueType::Current) const override;
    double getProgress(GameState& gameState, int& weight) const override;
    VariableChanges simulate(GameState& gameState) override;
    Node* normalize(bool negate = false) override;
    void bind() override {if (left) left->bind(); if (right) right->bind(); };

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override;
    std::set<std::string> getInputs(bool root, [[maybe_unused]] std::string function = "") const override;
    std::set<std::string> getOutputs([[maybe_unused]] bool root) const override;
    std::set<std::string> getBlockers(bool root) const override;

    bool isConstant(GameState& gameState) override;
    bool isConstantValue(GameState& gameState, double val) override;
    bool isRangeObject() override {if (left->isRangeObject() || right->isRangeObject()) return true; return false;};
    bool containEnumLikeVariable() const override;
    NodeType getType() override { return NodeType::Operand;};

    RangeObject getRangeObject() override;
    std::unique_ptr<Node> getRandomDistribution() override;

    std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const override;

    std::string getVarString() const override { return ""; };
};

struct FunctionNode : public Node{
    Insightable* target;
    std::vector<std::string> args;
    std::string name;
    std::unique_ptr<Node> clone() const override;

    FunctionNode() {};
    FunctionNode(std::string name, std::vector<std::string> args): name(name), args(args) {};

    double evaluate(GameState& gameState, ValueType valType = ValueType::Current) const override;
    double getProgress(GameState& gameState, int& weight) const override;
    VariableChanges simulate([[maybe_unused]] GameState& gameState) override { return VariableChanges(); };
    Node* normalize(bool negate = false) override;
    void bind() override;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    std::vector<DisplayLine> arithmeticalInsight(GameState& gameState, int level) override;

    std::set<std::string> getDependencies() const override;
    std::set<std::string> getInputs(bool root, [[maybe_unused]] std::string function = "") const override;
    std::set<std::string> getOutputs([[maybe_unused]] bool root) const override;
    std::set<std::string> getBlockers(bool root) const override;

    bool isConstant(GameState& GameState) override { return false; };
    bool isConstantValue(GameState& gameState, double val) override { return false; };
    bool isRangeObject() override { return false; };
    bool containEnumLikeVariable() const override { return false; };
    NodeType getType() override { return NodeType::Function; };

    RangeObject getRangeObject() override;
    std::unique_ptr<Node> getRandomDistribution() override;

    std::unique_ptr<Node> getPacketExpression(GameState& gameState, bool base) const override;

    std::string getVarString() const override { return ""; };
};

struct Expression{
    std::unique_ptr<Node> expr;
    std::set<std::string> variableLocks;
    Expression clone() const;

    Expression(const Expression& expression){
        Expression cloned = expression.clone();
        expr = std::move(cloned.expr);
        variableLocks = cloned.variableLocks;
    };

    Expression& operator=(const Expression& other) {
        if (this != &other) {
            Expression cloned = other.clone();
            expr = std::move(cloned.expr);
            variableLocks = other.variableLocks;
        }
        return *this;
    };

    Expression(std::unique_ptr<Node> expr, std::set<std::string> variableLocks): expr(std::move(expr)), variableLocks(variableLocks) {};

    double evaluate(GameState& gameState, ValueType valType = ValueType::Current){ return expr->evaluate(gameState, valType); };

    void normalize();

    std::set<std::string>& getVariableLocks() {return variableLocks;};
};

Expression construct(std::vector<Token> tokens);

#endif