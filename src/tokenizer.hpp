#ifndef tokenizer_hpp
#define tokenizer_hpp
#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <set>
#include "utils.hpp"

enum class Operand{
    NoOperand,
    Add, // +
    Subtract, // -
    Multiply, // *
    Divide, // /
    Modulo, // %

    Equal, // ==
    NotEqual, // !=
    Less, // <
    Greater, // >
    LessOrEqual, // <=
    GreaterOrEqual, // >=

    And, // &&
    Or, // ||
    Not, // !
    Neg, // -
    Abs, // abs()
    Max,
    Min,

    Assign, // =
    AddAssign, // +=
    SubAssign, // -=
    MulAssign, // *=
    DivAssign, // /=

    LeftPar,
    RightPar,
    If, // if left = condition right = what will happen
    Power // ^
};

enum class TokenType{
    Variable,
    VariableLock, // {var}
    Constant,
    Operand
};

enum class TokenizeState{
    Empty,
    Constant,
    Variable,
    
    Plus,
    Minus,
    Times,
    Divide,

    Less,
    Greater,
    Not,
    Equal,
    And,
    Or,

    CurlyBracket
};

class Token{
    private:
        TokenType type;
        std::variant<double, std::string, Operand> value;
        VariableFlags variableFlags;
    public:
        Token(double val): type(TokenType::Constant), value(val) {};
        Token(std::string name, bool varLock): type(varLock ? TokenType::VariableLock : TokenType::Variable), value(name) {};
        Token(Operand oper): type(TokenType::Operand), value(oper) {};
        void print();
        bool isValue() {if (type == TokenType::Constant || type == TokenType::Variable) return true; return false;};
        TokenType getType() {return type;};
        double getValueAsDouble() {return std::get<double>(value);};
        std::string getValueAsString() {return std::get<std::string>(value);};
        Operand getValueAsOperand() {return std::get<Operand>(value);};

        bool isSoft() {return type==TokenType::Variable && variableFlags.soft;};
        bool isFluid() {return type==TokenType::Variable && variableFlags.fluid;};
        bool isConstant() {return type==TokenType::Variable && variableFlags.constant;};
        bool isReal() {return type==TokenType::Variable && variableFlags.real;};

        void setFlags(VariableFlags flags) {variableFlags = flags;};
        VariableFlags getFlags() {return variableFlags;};
};

std::vector<Token> tokenize(std::string text);
bool isNumber(char c);
bool isLetter(char c);
int getPrecedence(Operand oper);

bool isFunction(Operand oper);
bool isLeftAssociative(Operand oper);
bool isUnary(Operand oper);
bool isAssignment(Operand oper);
bool isComparision(Operand oper);
bool isLogicalGate(Operand oper);

Operand getInverseOperand(Operand oper);

static const std::set<Operand> functionOperands {
    Operand::Abs, Operand::Min, Operand::Max, Operand::If
};

static const std::set<Operand> unaryOperands {
    Operand::Abs, Operand::Not, Operand::Neg
};

static const std::set<Operand> assignmentOperands {
    Operand::Assign, Operand::AddAssign, Operand::SubAssign, Operand::MulAssign, Operand::DivAssign
};

static const std::set<Operand> comparisionOperands {
    Operand::Equal, Operand::NotEqual, Operand::Less, Operand::LessOrEqual, Operand::Greater, Operand::GreaterOrEqual
};

static const std::set<Operand> logicalGates {
    Operand::And, Operand::Or
};

static const std::unordered_map<Operand, int> precedenceMap {
    {Operand::Power, 6},
    {Operand::Add, 3},
    {Operand::Subtract, 3},
    {Operand::Multiply, 4},
    {Operand::Divide, 4},
    {Operand::Modulo, 4},
    {Operand::Equal, 2},
    {Operand::NotEqual, 2},
    {Operand::Less, 2},
    {Operand::Greater, 2},
    {Operand::LessOrEqual, 2},
    {Operand::GreaterOrEqual, 2},
    {Operand::And, 2},
    {Operand::Or, 2},
    {Operand::Not, 5},
    {Operand::Neg, 5},
    {Operand::Abs, 5},
    {Operand::Min, 5},
    {Operand::Max, 5},
    {Operand::If, 5},
    {Operand::Assign, 0},
    {Operand::AddAssign, 0},
    {Operand::SubAssign, 0},
    {Operand::MulAssign, 0},
    {Operand::DivAssign, 0},
    {Operand::LeftPar, 1},
    {Operand::RightPar, 1}
};

static const std::unordered_map<char, TokenizeState> tokenizeStateMap {
    {'+', TokenizeState::Plus},
    {'-', TokenizeState::Minus},
    {'*', TokenizeState::Times},
    {'/', TokenizeState::Divide},
    {'<', TokenizeState::Less},
    {'>', TokenizeState::Greater},
    {'!', TokenizeState::Not},
    {'=', TokenizeState::Equal},
    {'&', TokenizeState::And},
    {'|', TokenizeState::Or}
};

static const std::unordered_map<std::string, Token> tokenMap {
    {"+", Token(Operand::Add)},
    {"+=", Token(Operand::AddAssign)},
    {"-", Token(Operand::Subtract)},
    {"-=", Token(Operand::SubAssign)},
    {"*", Token(Operand::Multiply)},
    {"*=", Token(Operand::MulAssign)},
    {"/", Token(Operand::Divide)},
    {"/=", Token(Operand::DivAssign)},
    {"<", Token(Operand::Less)},
    {"<=", Token(Operand::LessOrEqual)},
    {">", Token(Operand::Greater)},
    {">=", Token(Operand::GreaterOrEqual)},
    {"!", Token(Operand::Not)},
    {"!=", Token(Operand::NotEqual)},
    {"=", Token(Operand::Assign)},
    {"==", Token(Operand::Equal)},
    {"&&", Token(Operand::And)},
    {"||", Token(Operand::Or)},
    {")", Token(Operand::RightPar)},
    {"(", Token(Operand::LeftPar)},
    {"%", Token(Operand::Modulo)},
    {"^", Token(Operand::Power)},
    {"min", Token(Operand::Min)},
    {"max", Token(Operand::Max)},
    {"abs", Token(Operand::Abs)},
    {"if", Token(Operand::If)},
    {"true", Token(1)},
    {"false", Token(0)}
};

void printOperand(Operand oper);

#endif