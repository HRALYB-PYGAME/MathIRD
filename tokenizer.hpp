#ifndef tokenizer_hpp
#define tokenizer_hpp
#include <variant>
#include <vector>
#include <string>
#include <iostream>

enum class Operand{
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
    Power // ^
};

enum class TokenType{
    Variable,
    Constant,
    Operand
};

class Token{
    private:
        TokenType type;
        std::variant<double, std::string, Operand> value;
    public:
        Token(double val): type(TokenType::Constant), value(val) {};
        Token(std::string name): type(TokenType::Variable), value(name) {};
        Token(Operand oper): type(TokenType::Operand), value(oper) {};
        void print();
        bool isValue() {if (type == TokenType::Constant || type == TokenType::Variable) return true; return false;};
        TokenType getType() {return type;};
        double getValueAsDouble() {return std::get<double>(value);};
        std::string getValueAsString() {return std::get<std::string>(value);};
        Operand getValueAsOperand() {return std::get<Operand>(value);};
};

std::vector<Token> tokenize(std::string text);
bool isNumber(char c);
bool isLetter(char c);
int getPrecedence(Operand oper);

bool isFunction(Operand oper);
bool isLeftAssociative(Operand oper);
bool isUnary(Operand oper);
bool isAssignment(Operand oper);

#endif