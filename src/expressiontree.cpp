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

std::set<std::string> VariableNode::getOutputs(bool root){
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
        auto leftIns = this->left->getInputs(false);
        auto rightIns = this->right->getInputs(false);
        set.insert(leftIns.begin(), leftIns.end());
        set.insert(rightIns.begin(), rightIns.end());
    }
    return set;
}

std::set<std::string> OperandNode::getOutputs(bool root){
    std::set<std::string> set;
    if (isAssignment(this->oper)){
        auto leftOuts = this->left->getOutputs(false);
        set.insert(leftOuts.begin(), leftOuts.end());
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
    LOG("expressiontree.cpp\tsimulate() LEFT IS NOT NULL");
    if (this->left->getType() != NodeType::Variable) return VariableChanges();
    LOG("expressiontree.cpp\tsimulate() LEFT IS VAR");
    
    VariableNode* vNode = static_cast<VariableNode*>(this->left.get());
    std::string var = vNode->var;
    VariableChanges changes;

    if (isAssignment(oper) && !gameState.isVariableUnlocked(var)){
        LOG("expressiontree.cpp\tsimulate() " << var << " NOT UNLOCKED");
        return changes.add(var, 0);
    }

    double varValue = gameState.getVarValueAsDouble(var);

    gameState.forceRandom(0);
    double minValue = this->right->evaluate(gameState).getAsDouble();
    gameState.forceRandom(1);
    double maxValue = this->right->evaluate(gameState).getAsDouble();
    gameState.freeRandom();
    double randValue = this->right->evaluate(gameState).getAsDouble();

    LOG("expressiontree.cpp\tsimulate() VALUES CALCULATED minValue=" << minValue << " randValue=" << randValue << " maxValue=" << maxValue);

    switch(this->oper){
        case Operand::Assign:
            return changes.add(var, minValue-varValue, maxValue-varValue, randValue-varValue);
        case Operand::AddAssign:
            return changes.add(var, minValue, maxValue, randValue);
        case Operand::SubAssign:
            return changes.add(var, -minValue, -maxValue, -randValue);
        case Operand::MulAssign:
            return changes.add(var, minValue*varValue - varValue, maxValue*varValue - varValue, randValue*varValue - varValue);
        case Operand::DivAssign:
            return changes.add(var, varValue/minValue - varValue, varValue/maxValue - varValue, varValue/randValue - varValue);
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

std::vector<DisplayLine> Node::insight(GameState& gameState, int level){
    return { };
};

std::vector<DisplayLine> ConstantNode::insight(GameState& gameState, [[maybe_unused]] int level){
    double val = this->evaluate(gameState).getAsDouble();
    DisplayChunk c(formatDouble(val), DisplayType::Text);
    return { DisplayLine({ c }) };
};  

std::vector<DisplayLine> GeneralConstantNode::insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level){
    DisplayChunk c("C", DisplayType::Text);
    return { DisplayLine({ c }) };
}

std::vector<DisplayLine> VariableNode::insight(GameState& gameState, [[maybe_unused]] int level){
    std::string name = this->var;
    DisplayChunk c(name, DisplayType::Text);
    DisplayChunk h(name, DisplayType::Var);
    Variable* var = Defs::getVariable(name);
    if (var != nullptr) c.setLink(var);
    c.setHover({ h });
    return { DisplayLine({ c }) };
}

std::vector<DisplayLine> OperandNode::insight(GameState& gameState, int level){
    DisplayLine l;
    std::vector<DisplayLine> leftInsight;
    std::vector<DisplayLine> rightInsight;
    if (level == 0){
        leftInsight = left->insight(gameState, level+1);
        rightInsight = right->insight(gameState, level+1);
    }
    else{
        leftInsight = left->arithmeticalInsight(gameState, 0);
        rightInsight = right->arithmeticalInsight(gameState, 0);
    }
    switch(oper){
        case Operand::Add:
            return addInsight(*left, *right, gameState, level+1);
        case Operand::Subtract:
            return subtractInsight(*left, *right, gameState, level+1);
        case Operand::Multiply:
            l.appendTextChunk("Product of");
            l.appendLines(leftInsight);
            l.appendTextChunk("and");
            l.appendLines(rightInsight);
            return { l };
        case Operand::Assign:
            l.appendTextChunk("Set");
            l.appendLines(leftInsight);
            l.appendTextChunk("to");
            l.appendLines(rightInsight);
            return { l };
        default:
            return { l };
    }
    return { l };
}

std::vector<DisplayLine> ConstantNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    line.appendTextChunk(formatDouble(this->val.getAsDouble()));
    return { line };
}

std::vector<DisplayLine> GeneralConstantNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    line.appendTextChunk("C");
    return { line };
}

std::vector<DisplayLine> VariableNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    if (this->var == "_R" || this->var == "_NR"){
        line.appendTextChunk("R");
        return { line };
    }
    std::string name = this->var;
    DisplayChunk c(name, DisplayType::Text);
    DisplayChunk h(name, DisplayType::Var);
    Variable* var = Defs::getVariable(name);
    if (var != nullptr) c.setLink(var);
    c.setHover({ h });
    line.appendChunk(c);
    return { line };
}


std::vector<DisplayLine> OperandNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    if (level == 0) line.appendWordGapChunk(0.25);
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
        line.appendTextChunk("(-");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(")");
        break;
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
        line.appendTextChunk("(!");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(")");
        break;
    case Operand::Abs:
        line.appendTextChunk("|");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk("|");
        break;
    case Operand::Min:
        line.appendTextChunk("min(");
        line.appendLines(left->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(",");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(")");
        break;
    case Operand::Max:
        line.appendTextChunk("max(");
        line.appendLines(left->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(",");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(")");
        break;
    case Operand::If:
        line.appendTextChunk("if");
        line.appendLines(left->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk("do");
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        break;
    default:
        break;
    }
    if (operSymbol != ""){
        line.appendTextChunk("(");
        line.appendLines(left->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(operSymbol);
        line.appendLines(right->arithmeticalInsight(gameState, level+1));
        line.appendTextChunk(")");
    }
    if (level == 0){
        auto lastChunk = line.chunks.back();
        line.chunks.pop_back();
        line.appendWordGapChunk(1);
        line.appendChunk(lastChunk);
    }
    return {line};
}

void createSubtree(std::stack<std::unique_ptr<Node>>& nodeStack, Operand oper){
    LOG("expressiontree.cpp\tcreating subtree with nodestack of size: " << nodeStack.size());
    std::unique_ptr<Node> rightChild = std::move(nodeStack.top()); nodeStack.pop();
    std::unique_ptr<Node> leftChild = nullptr;
    if (!isUnary(oper)){
        leftChild = std::move(nodeStack.top()); nodeStack.pop();
    }
    auto parent = std::make_unique<OperandNode>(oper, std::move(leftChild), std::move(rightChild));
    nodeStack.push(std::move(parent));
    LOG("expressiontree.cpp\tcreating subtree with nodestack of size: " << nodeStack.size() << " succesfull");
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
    RangeObject l = left->getRangeObject();
    RangeObject r = right->getRangeObject();
    l.combine(r, oper); return l;
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

std::unique_ptr<Node> ConstantNode::clone() const{
    return std::make_unique<ConstantNode>(this->val);
}

std::unique_ptr<Node> VariableNode::clone() const{
    return std::make_unique<VariableNode>(this->var, this->soft);
}

std::unique_ptr<Node> OperandNode::clone() const{
    return std::make_unique<OperandNode>(this->oper, this->left->clone(), this->right->clone());
}

std::unique_ptr<Node> OperandNode::getPacketExpression(GameState& gameState) const{
    if (isAssignment(oper)){
        if (oper == Operand::Assign)
            return this->right->clone();
        if (oper == Operand::AddAssign)
            return std::make_unique<OperandNode>(OperandNode(Operand::Add, this->left->clone(), this->right->clone()));
        if (oper == Operand::SubAssign)
            return std::make_unique<OperandNode>(OperandNode(Operand::Subtract, this->left->clone(), this->right->clone()));
        if (oper == Operand::MulAssign)
            return std::make_unique<OperandNode>(OperandNode(Operand::Multiply, this->left->clone(), this->right->clone()));
        if (oper == Operand::DivAssign)
            return std::make_unique<OperandNode>(OperandNode(Operand::Divide, this->left->clone(), this->right->clone()));
    }
    return nullptr;
}