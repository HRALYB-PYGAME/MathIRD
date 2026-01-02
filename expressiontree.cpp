#include "expressiontree.hpp"
#include <stack>
#include "insight.hpp"

using namespace std::string_literals;

VariableValue ConstantNode::evaluate(GameState& gameState){
    return this->val;
}

VariableValue VariableNode::evaluate(GameState& gameState){
    return gameState.getVar(this->var)->getValue();
}

VariableChanges OperandNode::simulate(GameState& gameState){
    if (this->left == nullptr) return VariableChanges();
    if (this->left->getType() != NodeType::Variable) return VariableChanges();
    
    VariableNode* vNode = static_cast<VariableNode*>(this->left.get());
    std::string var = vNode->var;
    double varValue = gameState.getVar(var)->getValue().getAsDouble();

    double rightValue = this->right->evaluate(gameState).getAsDouble();
    VariableChanges changes;

    switch(this->oper){
        case Operand::Assign:
            return changes.add(var, rightValue-varValue);
        case Operand::AddAssign:
            return changes.add(var, rightValue);
        case Operand::SubAssign:
            return changes.add(var, -rightValue);
        case Operand::MulAssign:
            return changes.add(var, rightValue/varValue - varValue);
        case Operand::DivAssign:
            return changes.add(var, rightValue*varValue - varValue);
        default:
            return VariableChanges();
    }
}

VariableValue OperandNode::evaluate(GameState& gameState){
    VariableValue left(0);
    if (this->left == nullptr) left = VariableValue(0);
    else left = this->left->evaluate(gameState);
    VariableValue right = this->right->evaluate(gameState);
    VariableValue val(0);

    switch(this->oper){
    case Operand::Add:
        return left.add(right);
        break;
    case Operand::Subtract:
        return left.subtract(right);
        break;
    case Operand::Multiply:
        return left.multiply(right);
        break;
    case Operand::Divide:
        return left.divide(right);
        break;
    case Operand::Neg:
        return right.multiply(VariableValue(-1));
    case Operand::Assign:
        return right;
    case Operand::AddAssign:
        val = left.add(right);
        return val;
    case Operand::SubAssign:
        val = left.subtract(right);
        return val;
    case Operand::MulAssign:
        val = left.multiply(right);
        return val;
    case Operand::DivAssign:
        val = left.divide(right);
        return val;
    case Operand::Equal:
        if (left.isEqualTo(right)) return VariableValue(true);
        return VariableValue(false);
    case Operand::NotEqual:
        if (left.isEqualTo(right)) return VariableValue(false);
        return VariableValue(true);
    case Operand::Less:
        if (left.isLessThan(right)) return VariableValue(true);
        return VariableValue(false);
    case Operand::Greater:
        if (left.isGreaterThan(right)) return VariableValue(true);
        return VariableValue(false);
    case Operand::LessOrEqual:
        if (left.isLessThan(right) || left.isEqualTo(right)) return VariableValue(true);
        return VariableValue(false);
    case Operand::GreaterOrEqual:
        if (left.isGreaterThan(right) || left.isEqualTo(right)) return VariableValue(true);
        return VariableValue(false);
    case Operand::Modulo:
        return left.modulo(right);
    case Operand::Power:
        return left.power(right);
    case Operand::And:
        return VariableValue(left.logAnd(right));
    case Operand::Or:
        return VariableValue(left.logOr(right));
    case Operand::Not:
        return VariableValue(right.logNot());
    case Operand::Abs:
        if (right.isGreaterThan(VariableValue(0))) return right;
        return right.multiply(VariableValue(-1));
    case Operand::Min:
        if (right.isGreaterThan(left)) return left;
        return right;
    case Operand::Max:
        if (right.isGreaterThan(left)) return right;
        return left;
    }    

    return VariableValue(0);
}

std::string formatDouble(double val){
    std::string n = std::to_string(val);
    n.erase(n.find_last_not_of('0')+1, n.size()-1);
    if (n[n.size()-1] == '.') n.erase(n.size()-1);
    return n;
}

std::string ConstantNode::insight(GameState& gameState, int level){
    double val = this->evaluate(gameState).getAsDouble();
    return formatDouble(val)+RESET;
};

std::string VariableNode::insight(GameState& gameState, int level){
    std::string name = this->var;
    double val = this->evaluate(gameState).getAsDouble();
    return name.append(" (").append(formatDouble(val)).append(")");
}

std::string OperandNode::insight(GameState& gameState, int level){
    Operand oper = this->oper;
    double leftValue = this->left->evaluate(gameState).getAsDouble();
    double rightValue = this->right->evaluate(gameState).getAsDouble();
    std::string leftInsight = "";
    if (this->left != nullptr) leftInsight = this->left->insight(gameState, level+1);
    std::string rightInsight = this->right->insight(gameState, level+1);

    switch(oper){
    case Operand::Add:
        return addInsight(*this->left, *this->right, gameState, level);
    case Operand::Subtract:
        return subtractInsight(*this->left, *this->right, gameState, level);
    case Operand::Multiply:
        return multiplyInsight(*this->left, *this->right, gameState, level);    
    case Operand::Divide:
        return divideInsight(*this->left, *this->right, gameState, level);
    case Operand::Neg:
        return "negative " + rightInsight;
    case Operand::Assign:
        return "Set " + leftInsight + " to " + rightInsight;
    case Operand::AddAssign:
        return "Increase " + leftInsight + " by " + rightInsight;
    case Operand::SubAssign:
        return "Decrease " + leftInsight + " by " + rightInsight;
    case Operand::MulAssign:
        return "Multiply " + leftInsight + " by " + rightInsight;
    case Operand::DivAssign:
        return "Divide " + leftInsight + " by " + rightInsight;
    case Operand::Equal:
        return leftInsight + " is equal to " + rightInsight;
    case Operand::NotEqual:
        return leftInsight + " is not equal to " + rightInsight;
    case Operand::Less:
        return leftInsight + " is less than " + rightInsight;
    case Operand::Greater:
        return leftInsight + " is greater than " + rightInsight;
    case Operand::LessOrEqual:
        return leftInsight + " is at most " + rightInsight;
    case Operand::GreaterOrEqual:
        return leftInsight + " is at least " + rightInsight;
    case Operand::Modulo:
        return "remainder after dividing " + leftInsight + " by " + rightInsight;
    case Operand::Power:
        return leftInsight + " raised to the power of " + rightInsight;
    case Operand::And:
        return leftInsight + " and " + rightInsight;
    case Operand::Or:
        return leftInsight + " or " + rightInsight;
    case Operand::Not:
        return "not " + rightInsight;
    case Operand::Abs:
        return "the absolute value of " + rightInsight;
    case Operand::Min:
        return "minimum of " + leftInsight + " and " + rightInsight;
    case Operand::Max:
        return "maximum of " + leftInsight + " and " + rightInsight;
    }
}

void createSubtree(std::stack<std::unique_ptr<Node>>& nodeStack, Operand oper){
    std::unique_ptr<Node> rightChild = std::move(nodeStack.top()); nodeStack.pop();
    std::unique_ptr<Node> leftChild = nullptr;
    if (!isUnary(oper)){
        leftChild = std::move(nodeStack.top()); nodeStack.pop();
    }
    auto parent = std::make_unique<OperandNode>(oper, std::move(leftChild), std::move(rightChild));
    nodeStack.push(std::move(parent));
}

void printNodeStack(std::stack<std::unique_ptr<Node>>& nodeStack) {
    std::cout << nodeStack.size() << std::endl;
}

std::unique_ptr<Node> construct(std::vector<Token> tokens){
    std::cout << "construct\n";
    std::stack<std::unique_ptr<Node>> nodeStack;
    std::stack<Operand> operandStack;
    for(int i=0; i<tokens.size(); i++){
        Token token = tokens[i];
        token.print();
        switch(token.getType()){
        case TokenType::Constant:
            nodeStack.push(std::make_unique<ConstantNode>(VariableValue(token.getValueAsDouble())));
            break;
        case TokenType::Variable:
            nodeStack.push(std::make_unique<VariableNode>(token.getValueAsString()));
            break;
        case TokenType::Operand:
            Operand oper = token.getValueAsOperand();
            if (isFunction(oper)){
                operandStack.push(oper);
            }
            else if (oper == Operand::LeftPar){
                operandStack.push(oper);
            }
            else if (oper == Operand::RightPar){
                while(operandStack.top() != Operand::LeftPar){
                    Operand topOper = operandStack.top(); operandStack.pop();
                    createSubtree(nodeStack, topOper);
                }
                operandStack.pop();
                if (!operandStack.empty() && isFunction(operandStack.top())){
                    Operand fun = operandStack.top();
                    createSubtree(nodeStack, fun);
                    operandStack.pop();
                }
            }
            else{
                while(!operandStack.empty() && (getPrecedence(operandStack.top()) > getPrecedence(oper) ||
                    (getPrecedence(operandStack.top()) == getPrecedence(oper) && isLeftAssociative(operandStack.top())))){
                    Operand topOper = operandStack.top(); operandStack.pop();
                    createSubtree(nodeStack, topOper);
                }
                operandStack.push(oper);
            }
        }
        printNodeStack(nodeStack);
    }

    while(!operandStack.empty()){
        Operand topOper = operandStack.top(); operandStack.pop();
        Token(topOper).print();
        createSubtree(nodeStack, topOper);
        printNodeStack(nodeStack);
    }

    return std::move(nodeStack.top());
}