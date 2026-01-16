#include "tokenizer.hpp"

void Token::print(){
    switch(this->type){
        case TokenType::Constant:
            std::cout << "Token Type: Constant (" << std::get<double>(this->value) << ")\n";
            break;
        case TokenType::Operand:
            std::cout << "Token Type: Operand ";
            switch(std::get<Operand>(this->value)){
                case Operand::Add:
                    std::cout << "(+)" << std::endl;
                    break;
                case Operand::Subtract:
                    std::cout << "(-)" << std::endl;
                    break;
                case Operand::Multiply:
                    std::cout << "(*)" << std::endl;
                    break;
                case Operand::Divide:
                    std::cout << "(/)" << std::endl;
                    break;
                case Operand::AddAssign:
                    std::cout << "(+=)" << std::endl;
                    break;
                case Operand::SubAssign:
                    std::cout << "(-=)" << std::endl;
                    break;
                case Operand::MulAssign:
                    std::cout << "(*=)" << std::endl;
                    break;
                case Operand::DivAssign:
                    std::cout << "(/=)" << std::endl;
                    break;
                case Operand::Modulo:
                    std::cout << "(%)" << std::endl;
                    break;
                case Operand::Equal:
                    std::cout << "(==)" << std::endl;
                    break;
                case Operand::NotEqual:
                    std::cout << "(!=)" << std::endl;
                    break;
                case Operand::Less:
                    std::cout << "(<)" << std::endl;
                    break;
                case Operand::Greater:
                    std::cout << "(>)" << std::endl;
                    break;
                case Operand::LessOrEqual:
                    std::cout << "(<=)" << std::endl;
                    break;
                case Operand::GreaterOrEqual:
                    std::cout << "(>=)" << std::endl;
                    break;
                case Operand::And:
                    std::cout << "(&&)" << std::endl;
                    break;
                case Operand::Or:
                    std::cout << "(||)" << std::endl;
                    break;
                case Operand::Not:
                    std::cout << "(!)" << std::endl;
                    break;
                case Operand::Neg:
                    std::cout << "(neg)" << std::endl;
                    break;
                case Operand::Abs:
                    std::cout << "(abs)" << std::endl;
                    break;
                case Operand::Max:
                    std::cout << "(max)" << std::endl;
                    break;
                case Operand::Min:
                    std::cout << "(min)" << std::endl;
                    break;
                case Operand::Assign:
                    std::cout << "(=)" << std::endl;
                    break;
                case Operand::LeftPar:
                    std::cout << "('(')" << std::endl;
                    break;
                case Operand::RightPar:
                    std::cout << "(')')" << std::endl;
                    break;
                case Operand::Power:
                    std::cout << "(^)" << std::endl;
                    break;
                case Operand::If:
                    std::cout << "(if)" << std::endl;
                    break;
                case Operand::NoOperand:
                    std::cout << "(?)" << std::endl;
                    break;
            }
            break;
        case TokenType::Variable:
            std::cout << "Token Type: Variable (" << std::get<std::string>(this->value) << ")\n";
            break;
        case TokenType::SoftVariable:
            std::cout << "Token Type: ~Variable (" << std::get<std::string>(this->value) << ")\n";
            break;
    }
}

bool isNumber(char c){
    if (c >= '0' && c <= '9') return true;
    return false;
}

bool isLetter(char c){
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '~')
        return true;
    return false;
}

int getPrecedence(Operand oper){
    switch(oper){
    case Operand::Power:
        return 6;
        break;
    case Operand::Add:
        return 3;
        break;
    case Operand::Subtract:
        return 3;
        break;
    case Operand::Multiply:
        return 4;
        break;
    case Operand::Divide:
        return 4;
        break;
    case Operand::Modulo:
        return 4;
        break;
    case Operand::Equal:
        return 2;
        break;
    case Operand::NotEqual:
        return 2;
        break;
    case Operand::Less:
        return 2;
        break;
    case Operand::Greater:
        return 2;
        break;
    case Operand::LessOrEqual:
        return 2;
        break;
    case Operand::GreaterOrEqual:
        return 2;
        break;
    case Operand::And:
        return 2;
        break;
    case Operand::Or:
        return 2;
        break;
    case Operand::Not:
        return 5;
        break;
    case Operand::Neg:
        return 5;
        break;
    case Operand::Abs:
        return 5;
        break;
    case Operand::Min:
        return 5;
        break;
    case Operand::Max:
        return 5;
        break;
    case Operand::If:
        return 5;
        break;
    case Operand::Assign:
        return 0;
        break;
    case Operand::AddAssign:
        return 0;
        break;
    case Operand::SubAssign:
        return 0;
        break;
    case Operand::MulAssign:
        return 0;
        break;
    case Operand::DivAssign:
        return 0;
        break;
    case Operand::LeftPar:
        return -1;
        break;
    case Operand::RightPar:
        return -1;
        break;
    default:
        return 0;
    }
}

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
    Or
};

std::vector<Token> tokenize(std::string text){
    text = text.append(" ");
    TokenizeState state = TokenizeState::Empty;
    std::string acc = "";
    std::vector<Token> tokens;

    for(size_t i=0; i<text.length(); i++){
        char c = text[i];
        switch(state){
        case TokenizeState::Empty:
            if (isNumber(c)){
                acc += c;
                state = TokenizeState::Constant;
            }
            else if (c == '['){
                tokens.insert(tokens.end(), Operand::LeftPar);
                tokens.insert(tokens.end(), Operand::LeftPar);
            }
            else if (c == ']'){
                tokens.insert(tokens.end(), Operand::RightPar);
                tokens.insert(tokens.end(), Operand::RightPar);
            }
            else if (c == ','){
                tokens.insert(tokens.end(), Operand::RightPar);
                tokens.insert(tokens.end(), Operand::LeftPar);
            }
            else if (isLetter(c) || c == '_'){
                acc += c;
                state = TokenizeState::Variable;
            }
            else if (c == '+'){
                acc += c;
                state = TokenizeState::Plus;
            }
            else if (c == '-'){
                acc += c;
                state = TokenizeState::Minus;
            }
            else if (c == '*'){
                acc += c;
                state = TokenizeState::Times;
            }
            else if (c == '/'){
                acc += c;
                state = TokenizeState::Divide;
            }
            else if (c == '<'){
                acc += c;
                state = TokenizeState::Less;
            }
            else if (c == '>'){
                acc += c;
                state = TokenizeState::Greater;
            }
            else if (c == '!'){
                acc += c;
                state = TokenizeState::Not;
            }
            else if (c == '='){
                acc += c;
                state = TokenizeState::Equal;
            }
            else if (c == '&'){
                acc += c;
                state = TokenizeState::And;
            }
            else if (c == '|'){
                acc += c;
                state = TokenizeState::Or;
            }
            else if (c == ')'){
                tokens.insert(tokens.end(), Operand::RightPar);
            }
            else if (c == '('){
                tokens.insert(tokens.end(), Operand::LeftPar);
            }
            else if (c == '%'){
                tokens.insert(tokens.end(), Operand::Modulo);
            }
            else if (c == '^'){
                tokens.insert(tokens.end(), Operand::Power);
            }
            break;
        case TokenizeState::Constant:
            if (isNumber(c) || c == '.'){
                acc += c;
            }
            else{
                tokens.insert(tokens.end(), Token(stod(acc)));
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Variable:
            if (isLetter(c) || c == '_' || isNumber(c)){
                acc += c;
            }
            else{
                if (acc.compare("min") == 0)
                    tokens.insert(tokens.end(), Token(Operand::Min));
                else if (acc.compare("max") == 0)
                    tokens.insert(tokens.end(), Token(Operand::Max));
                else if (acc.compare("abs") == 0)
                    tokens.insert(tokens.end(), Token(Operand::Abs));
                else if (acc.compare("true") == 0)
                    tokens.insert(tokens.end(), Token(1));
                else if (acc.compare("false") == 0)
                    tokens.insert(tokens.end(), Token(0));
                else if (acc.compare("if") == 0)
                    tokens.insert(tokens.end(), Token(Operand::If));
                else if (acc[0] == '~')
                    tokens.insert(tokens.end(), Token(acc.substr(1), true));
                else
                    tokens.insert(tokens.end(), Token(acc, false));
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Plus:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::AddAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Add);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Minus:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::SubAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                if (tokens.empty() || !tokens[tokens.size()-1].isValue())
                    tokens.insert(tokens.end(), Operand::Neg);
                else
                    tokens.insert(tokens.end(), Operand::Subtract);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Times:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::MulAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Multiply);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Divide:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::DivAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Divide);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Less:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::LessOrEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Less);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Greater:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::GreaterOrEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Greater);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Not:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::NotEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Not);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Equal:
            if (c == '='){
                tokens.insert(tokens.end(), Operand::Equal);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.insert(tokens.end(), Operand::Assign);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::And:
            if (c == '&'){
                tokens.insert(tokens.end(), Operand::And);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Or:
            if (c == '|'){
                tokens.insert(tokens.end(), Operand::Or);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        }
        
    }
    return tokens;
}

bool isFunction(Operand oper){
    if (oper == Operand::Abs || oper == Operand::Min || oper == Operand::Max || oper == Operand::If)
        return true;
    return false;
}

bool isLeftAssociative(Operand oper){
    if (oper == Operand::Power || isFunction(oper) || isUnary(oper) || isAssignment(oper))
        return false;
    return true;
}

bool isUnary(Operand oper){
    if (oper == Operand::Abs || oper == Operand::Not || oper == Operand::Neg)
        return true;
    return false;
}

bool isAssignment(Operand oper){
    if (oper == Operand::Assign || oper == Operand::AddAssign || oper == Operand::SubAssign || oper == Operand::MulAssign || oper == Operand::DivAssign)
        return true;
    return false;
}