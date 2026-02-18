#include "tokenizer.hpp"

void Token::print(){
    std::string flags = "";
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
            flags.clear();
            if (isSoft()) flags += "Soft ";
            if (isFluid()) flags += "Fluid ";
            if (isConstant()) flags += "Constant ";
            if (isReal()) flags += "Real ";
            std::cout << "Token Type: " << flags << "Variable (" << std::get<std::string>(this->value) << ")\n";
            break;
        case TokenType::VariableLock:
            std::cout << "Token Type: VariableLock (" << std::get<std::string>(this->value) << ")\n";
            break;
        case TokenType::Function:
            std::cout << "Function: (" << std::get<std::string>(this->value) << ")\n";
            break;
        case TokenType::Argument:
            std::cout << "Argument: (" << std::get<std::string>(this->value) << ")\n";
    }
}

bool isNumber(char c){
    if (c >= '0' && c <= '9') return true;
    return false;
}

bool isLetter(char c){
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    return false;
}

bool isVarFlagChar(char c){
    if (c == '~' || c == '$' || c == '?')
        return true;
    return false;
}

int getPrecedence(Operand oper){
    auto it = precedenceMap.find(oper);
    if (it == precedenceMap.end()) return 0;
    return it->second;
}

std::string stateExplanation(TokenizeState state){
    switch(state){
    case TokenizeState::Empty:
        return "Empty";
    case TokenizeState::Constant:
        return "Constant";
    case TokenizeState::Variable:
        return "Variable";
        
    case TokenizeState::Plus:
        return "Plus";
    case TokenizeState::Minus:
        return "Minus";
    case TokenizeState::Times:
        return "Times";
    case TokenizeState::Divide:
        return "Divide";

    case TokenizeState::Less:
        return "Less";
    case TokenizeState::Greater:
        return "Greater";
    case TokenizeState::Not:
        return "Not";
    case TokenizeState::Equal:
        return "Equal";
    case TokenizeState::And:
        return "And";
    case TokenizeState::Or:
        return "Or";

    case TokenizeState::CurlyBracket:
        return "CurlyBracket";
    case TokenizeState::Function:
        return "Function";
    }

    return "UnknownState";
}

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
                tokens.push_back(Operand::LeftPar);
                tokens.push_back(Operand::LeftPar);
            }
            else if (c == ']'){
                tokens.push_back(Operand::RightPar);
                tokens.push_back(Operand::RightPar);
            }
            else if (c == ','){
                tokens.push_back(Operand::RightPar);
                tokens.push_back(Operand::LeftPar);
            }
            else if (c == '{'){
                state = TokenizeState::CurlyBracket;
            }
            else if (c == '('){
                tokens.push_back(Operand::LeftPar);
            }
            else if (c == ')'){
                tokens.push_back(Operand::RightPar);
            }
            else if (isLetter(c) || c == '_' || isVarFlagChar(c)){
                acc += c;
                state = TokenizeState::Variable;
            }
            else if (tokenizeStateMap.count(c)){
                acc += c;
                state = tokenizeStateMap.at(c);
            }
            else if (tokenMap.count(std::to_string(c))){
                std::cout << "found" << std::endl;
                tokens.push_back(tokenMap.at(std::to_string(c)));
            }
            break;
        case TokenizeState::Constant:
            if (isNumber(c) || c == '.'){
                acc += c;
            }
            else{
                tokens.push_back(Token(stod(acc)));
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Function:
            if (isLetter(c) || c == '_' || isNumber(c) || isVarFlagChar(c) || c == ':' || c == '.'){
                acc += c;
            }
            else if (c == ']'){
                tokens.push_back(Token(acc, TokenType::Argument));
                tokens.push_back(Operand::RightPar);
                acc.clear();
                state = TokenizeState::Empty;
            }
            else if (c == ','){
                tokens.push_back(Token(acc, TokenType::Argument));
                acc.clear();
            }
            break;
        case TokenizeState::Variable:
            if (isLetter(c) || c == '_' || isNumber(c) || isVarFlagChar(c)){
                acc += c;
            }
            else if (c == '['){
                tokens.push_back(Token(acc, TokenType::Function));
                tokens.push_back(Operand::LeftPar);
                acc.clear();
                state = TokenizeState::Function;
            }
            else{
                if (tokenMap.count(acc))
                    tokens.push_back(tokenMap.at(acc));
                else if (acc.size() == 1)
                    tokens.push_back(Token(acc, TokenType::Variable));
                else{
                    VariableFlags flags;
                    while (acc.size() > 0 && (acc[0] == '~' || acc[0] == '$')){
                        if (acc[0] == '~') flags.setSoft();
                        if (acc[0] == '$') flags.setConstant();
                        acc.erase(0, 1);
                    }
                    while (acc.size() > 0 && (acc.back() == '~' || acc.back() == '?')){
                        if (acc.back() == '~') flags.setFluid();
                        if (acc.back() == '?') flags.setReal();
                        acc.pop_back();
                    }

                    Token varToken(acc, TokenType::Variable);
                    varToken.setFlags(flags);
                    tokens.push_back(varToken);
                }
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::CurlyBracket:
            if (c == '_' || isLetter(c) || (isNumber(c) && !acc.empty()))
                acc += c;
            if (c == ',' && !acc.empty()){
                tokens.push_back(Token(acc, TokenType::VariableLock));
                acc.clear();
            }
            if (c == '}'){
                if (!acc.empty()) tokens.push_back(Token(acc, TokenType::VariableLock));
                acc.clear();
                state = TokenizeState::Empty;
            }
            break;
        case TokenizeState::Plus:
            if (c == '='){
                tokens.push_back(Operand::AddAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Add);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Minus:
            if (c == '='){
                tokens.push_back(Operand::SubAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                if (tokens.empty() || !tokens[tokens.size()-1].isValue())
                    tokens.push_back(Operand::Neg);
                else
                    tokens.push_back(Operand::Subtract);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Times:
            if (c == '='){
                tokens.push_back(Operand::MulAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Multiply);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Divide:
            if (c == '='){
                tokens.push_back(Operand::DivAssign);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Divide);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Less:
            if (c == '='){
                tokens.push_back(Operand::LessOrEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Less);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Greater:
            if (c == '='){
                tokens.push_back(Operand::GreaterOrEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Greater);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Not:
            if (c == '='){
                tokens.push_back(Operand::NotEqual);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Not);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::Equal:
            if (c == '='){
                tokens.push_back(Operand::Equal);
                state = TokenizeState::Empty;
                acc.clear();
            }
            else {
                tokens.push_back(Operand::Assign);
                state = TokenizeState::Empty;
                acc.clear();
                i--;
            }
            break;
        case TokenizeState::And:
            if (c == '&'){
                tokens.push_back(Operand::And);
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
                tokens.push_back(Operand::Or);
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
    for(auto token : tokens){
        token.print();
    }
    return tokens;
}

bool isFunction(Operand oper){
    return functionOperands.count(oper);
}

bool isLeftAssociative(Operand oper){
    if (oper == Operand::Power || isFunction(oper) || isUnary(oper) || isAssignment(oper))
        return false;
    return true;
}

bool isUnary(Operand oper){
    return unaryOperands.count(oper);
}

bool isAssignment(Operand oper){
    return assignmentOperands.count(oper);
}

bool isComparision(Operand oper){
    return comparisionOperands.count(oper);
}

bool isLogicalGate(Operand oper){
    return logicalGates.count(oper);
}

Operand getInverseOperand(Operand oper){
    if (oper == Operand::Equal) return Operand::NotEqual;
    if (oper == Operand::NotEqual) return Operand::Equal;
    if (oper == Operand::LessOrEqual) return Operand::Greater;
    if (oper == Operand::Less) return Operand::GreaterOrEqual;
    if (oper == Operand::GreaterOrEqual) return Operand::Less;
    if (oper == Operand::Greater) return Operand::LessOrEqual;
    return oper;
}

void printOperand(Operand oper){
    switch(oper){
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
}