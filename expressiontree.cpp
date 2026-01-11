#include "expressiontree.hpp"
#include <stack>
#include "insight.hpp"

using namespace std::string_literals;

std::set<std::string> VariableNode::getDependencies(){
    std::set<std::string> deps;
    if (this->soft) return deps;
    deps.insert(this->var);
    return deps;
}

std::set<std::string> OperandNode::getDependencies(){
    auto leftDeps = this->left->getDependencies();
    auto rightDeps = this->right->getDependencies();
    leftDeps.insert(rightDeps.begin(), rightDeps.end());
    return leftDeps;
}

std::set<std::string> VariableNode::getInputs(bool root){
    std::set<std::string> set;
    if (!root){
        set.insert(this->var);
    }
    return set;
}

std::set<std::string> OperandNode::getInputs(bool root){
    std::set<std::string> set;
    if (root){
        if (isAssignment(this->oper)){
            auto rightIns = this->right->getInputs(false);
            set.insert(rightIns.begin(), rightIns.end());
        }
    }
    else {
        auto leftIns = this->right->getInputs(false);
        auto rightIns = this->left->getInputs(false);
        set.insert(leftIns.begin(), leftIns.end());
        set.insert(rightIns.begin(), rightIns.end());
    }
    return set;
}

bool VariableNode::isConstant(GameState& gameState){
    return !gameState.isVariableUnlocked(this->var);
}

bool OperandNode::isConstant(GameState& gameState){
    if (this->left->isConstant(gameState) && this->right->isConstant(gameState))
        return true;
    return false;
}

bool ConstantNode::isConstantValue([[maybe_unused]] GameState& gameState, double val){
    if (this->val.getAsDouble() == val) return true;
    return false;
}

bool VariableNode::isConstantValue(GameState& gameState, double val){
    if (this->isConstant(gameState) && this->evaluate(gameState).getAsDouble() == val)
        return true;
    return false;
}

bool OperandNode::isConstantValue(GameState& gameState, double val){
    if (this->isConstant(gameState) && this->evaluate(gameState).getAsDouble() == val)
        return true;
    return false;
}

VariableValue ConstantNode::evaluate([[maybe_unused]] GameState& gameState){
    return this->val;
}

VariableValue VariableNode::evaluate(GameState& gameState){
    return gameState.getVarValue(this->var);
}

VariableChanges OperandNode::simulate(GameState& gameState){
    if (this->left == nullptr) return VariableChanges();
    if (this->left->getType() != NodeType::Variable) return VariableChanges();
    
    VariableNode* vNode = static_cast<VariableNode*>(this->left.get());
    std::string var = vNode->var;
    VariableChanges changes;

    if (isAssignment(oper) && !gameState.isVariableUnlocked(var))
        return changes.add(var, 0);

    double varValue = gameState.getVarValueAsDouble(var);

    double rightValue = this->right->evaluate(gameState).getAsDouble();

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
        case Operand::If:
            if (left->evaluate(gameState).getAsBool())
                return this->right->simulate(gameState);
            return changes;
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
    case Operand::If:
        if (left.getAsBool())
            return right;
        return VariableValue(0);
    default:
        return VariableValue(0);
    }
}

std::string ConstantNode::insight(GameState& gameState, [[maybe_unused]] int level){
    double val = this->evaluate(gameState).getAsDouble();
    return formatDouble(val)+RESET;
};

std::string VariableNode::insight(GameState& gameState, [[maybe_unused]] int level){
    std::string name = this->var;
    if (!gameState.isVariableUnlocked(name))
        return formatDouble(gameState.getVarValueAsDouble(name));
    double val = this->evaluate(gameState).getAsDouble();
    return name.append(" (").append(formatDouble(val)).append(")");
}

std::string OperandNode::insight(GameState& gameState, int level){
    if (this->isRangeObject() && level != 0){
        RangeObject ro = this->getRangeObject();
        std::string distributionInsight = "";
        if (ro.distribution->getType() != NodeType::Variable)
            distributionInsight = ro.distribution->arithmeticalInsight();
        return "random number between " + ro.min->insight(gameState, level+1) + " and " + ro.max->insight(gameState, level+1)
            + " " + distributionInsight;
    }

    if (isAssignment(oper) && this->left->isConstant(gameState)){
        return "";
    }

    Operand oper = this->oper;
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
    case Operand::If:
        if (left->isRangeObject()){
            gameState.insertNewConditionResult(left->evaluate(gameState).getAsBool());
            double prob = gameState.insertNewConditionResult(left->evaluate(gameState).getAsBool());
            return std::to_string((int)(prob*100)) + "% of the time " + rightInsight;
        }
        return "if " + leftInsight + " then " + rightInsight;
    default:
        return "";
    }
}

std::string OperandNode::arithmeticalInsight(){
    std::string operSymbol = "";
    switch(oper){
    case Operand::Add:
        operSymbol = "+";
        break;
    case Operand::Subtract:
        operSymbol = "-";
        break;
    case Operand::Multiply:
        operSymbol = "*"; 
        break;
    case Operand::Divide:
        operSymbol = "/";
        break;
    case Operand::Neg:
        return "(-" + right->arithmeticalInsight() + ")";
    case Operand::Assign:
        operSymbol = "=";
        break;
    case Operand::AddAssign:
        operSymbol = "+=";
        break;
    case Operand::SubAssign:
        operSymbol = "-=";
        break;
    case Operand::MulAssign:
        operSymbol = "*=";
        break;
    case Operand::DivAssign:
        operSymbol = "/=";
        break;
    case Operand::Equal:
        operSymbol = "==";
        break;
    case Operand::NotEqual:
        operSymbol = "!=";
        break;
    case Operand::Less:
        operSymbol = "<";
        break;
    case Operand::Greater:
        operSymbol = ">";
        break;
    case Operand::LessOrEqual:
        operSymbol = "<=";
        break;
    case Operand::GreaterOrEqual:
        operSymbol = ">=";
        break;
    case Operand::Modulo:
        operSymbol = "%";
        break;
    case Operand::Power:
        operSymbol = "^";
        break;
    case Operand::And:
        operSymbol = "&&";
        break;
    case Operand::Or:
        operSymbol = "||";
        break;
    case Operand::Not:
        return "(!" + right->arithmeticalInsight() + ")";
    case Operand::Abs:
        return "|" + right->arithmeticalInsight() + "|";
    case Operand::Min:
        return "min(" + left->arithmeticalInsight() + ", " + right->arithmeticalInsight() + ")";
    case Operand::Max:
        return "max(" + left->arithmeticalInsight() + ", " + right->arithmeticalInsight() + ")";
    case Operand::If:
        return "if " + left->arithmeticalInsight() + " do " + right->arithmeticalInsight();
    default:
        return "";
    }
    return "(" + left->arithmeticalInsight() + operSymbol + right->arithmeticalInsight() + ")";
}

void createSubtree(std::stack<std::unique_ptr<Node>>& nodeStack, Operand oper){
    //std::cout << "creating subtree with nodestack of size: " << nodeStack.size() << std::endl;
    std::unique_ptr<Node> rightChild = std::move(nodeStack.top()); nodeStack.pop();
    //std::cout << "successfully popped right child\n";
    std::unique_ptr<Node> leftChild = nullptr;
    if (!isUnary(oper)){
        leftChild = std::move(nodeStack.top()); nodeStack.pop();
    }
    auto parent = std::make_unique<OperandNode>(oper, std::move(leftChild), std::move(rightChild));
    nodeStack.push(std::move(parent));
    //std::cout << std::endl;
}

void printNodeStack(std::stack<std::unique_ptr<Node>>& nodeStack) {
    std::cout << nodeStack.size() << std::endl;
}

std::unique_ptr<Node> construct(std::vector<Token> tokens){
    std::stack<std::unique_ptr<Node>> nodeStack;
    std::stack<Operand> operandStack;
    for(size_t i=0; i<tokens.size(); i++){
        Token token = tokens[i];
        //std::cout << "nodestack size " << nodeStack.size() << " operandStack size " << operandStack.size() << std::endl;
        //token.print();
        switch(token.getType()){
        case TokenType::Constant:
            nodeStack.push(std::make_unique<ConstantNode>(VariableValue(token.getValueAsDouble())));
            break;
        case TokenType::SoftVariable:
            nodeStack.push(std::make_unique<VariableNode>(token.getValueAsString(), true));
            break;
        case TokenType::Variable:
            nodeStack.push(std::make_unique<VariableNode>(token.getValueAsString(), false));
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
    }

    while(!operandStack.empty()){
        Operand topOper = operandStack.top(); operandStack.pop();
        createSubtree(nodeStack, topOper);
    }

    return std::move(nodeStack.top());
}

RangeObject OperandNode::getRangeObject(){
    //printf("1");
    RangeObject l = left->getRangeObject();
    //printf("2");
    RangeObject r = right->getRangeObject();
    //printf("3");
    l.combine(r, oper); return l;
    //printf("4");
};

std::unique_ptr<Node> ConstantNode::getRandomDistribution() { 
    return std::make_unique<GeneralConstantNode>(); 
}

std::unique_ptr<Node> VariableNode::getRandomDistribution() { 
    if (this->var == "_R" or this->var == "_NR")
        return std::make_unique<VariableNode>("_R", false);
    return std::make_unique<GeneralConstantNode>(); 
}

std::unique_ptr<Node> OperandNode::getRandomDistribution() { 
    std::unique_ptr<Node> leftRD  = this->left ->getRandomDistribution();
    std::unique_ptr<Node> rightRD = this->right->getRandomDistribution();

    if (leftRD->getType() == NodeType::GeneralConstant && rightRD->getType() == NodeType::GeneralConstant)
        return std::make_unique<GeneralConstantNode>();
    if (oper == Operand::Power)
        return std::make_unique<OperandNode>(oper, std::move(leftRD), std::move(rightRD));
    if (leftRD->getType() == NodeType::GeneralConstant)
        return rightRD;
    if (rightRD->getType() == NodeType::GeneralConstant)
        return leftRD;
    if (leftRD->getType() == NodeType::Variable && rightRD->getType() == NodeType::Variable && (oper == Operand::Add || oper == Operand::Subtract))
        return std::make_unique<VariableNode>("_R", false);
    return std::make_unique<OperandNode>(oper, std::move(leftRD), std::move(rightRD));
}