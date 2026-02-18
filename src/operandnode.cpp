#include "expressiontree.hpp"
#include "game_state.hpp"
#include "insight.hpp"

std::set<std::string> OperandNode::getDependencies() const{
    auto leftDeps = this->left->getDependencies();
    auto rightDeps = this->right->getDependencies();
    leftDeps.insert(rightDeps.begin(), rightDeps.end());
    return leftDeps;
}

std::set<std::string> OperandNode::getInputs(bool root, std::string function) const{
    std::set<std::string> set;
    if (root){
        if (isAssignment(oper)){
            auto rightIns = right->getInputs(false);
            set.insert(rightIns.begin(), rightIns.end());
        }
    }
    else {
        auto leftIns = left->getInputs(false);
        auto rightIns = right->getInputs(false);
        set.insert(leftIns.begin(), leftIns.end());
        set.insert(rightIns.begin(), rightIns.end());
    }
    return set;
}

std::set<std::string> OperandNode::getOutputs(bool root) const{
    std::set<std::string> set;
    if (isAssignment(oper)){
        auto leftOuts = left->getOutputs(false);
        set.insert(leftOuts.begin(), leftOuts.end());
    }
    return set;
}

std::set<std::string> OperandNode::getBlockers(bool root) const{
    std::set<std::string> set;
    if (isAssignment(oper) || !root){
        auto leftBlocks = left->getBlockers(false);
        auto rightBlocks = right->getBlockers(false);
        set.insert(leftBlocks.begin(), leftBlocks.end());
        set.insert(rightBlocks.begin(), rightBlocks.end());
    }
    return set;
}

bool OperandNode::isConstant(GameState& gameState){
    if (this->left->isConstant(gameState) && this->right->isConstant(gameState))
        return true;
    return false;
}

bool OperandNode::isConstantValue(GameState& gameState, double val){
    if (this->isConstant(gameState) && this->evaluate(gameState) == val)
        return true;
    return false;
}

bool OperandNode::containEnumLikeVariable() const{
    bool l = left ?  left->containEnumLikeVariable()  : false;
    bool r = right ? right->containEnumLikeVariable() : false;
    return l || r;
}

VariableChanges OperandNode::simulate(GameState& gameState){
    if (this->left == nullptr) return VariableChanges();
    LOG("expressiontree.cpp\tsimulate() LEFT IS NOT NULL");
    if (this->left->getType() != NodeType::Variable) return VariableChanges();
    LOG("expressiontree.cpp\tsimulate() LEFT IS VAR");
    
    //VariableNode* vNode = static_cast<VariableNode*>(this->left.get());
    std::string var = left->getVarString();
    if (var == "") return VariableChanges();
    VariableChanges changes;

    if (isAssignment(oper) && !gameState.isVariableUnlocked(var)){
        LOG("expressiontree.cpp\tsimulate() " << var << " NOT UNLOCKED");
        return changes.add(var, 0);
    }

    double varValue = gameState.getVarValue(var);

    gameState.forceRandom(0);
    double minValue = this->right->evaluate(gameState);
    gameState.forceRandom(1);
    double maxValue = this->right->evaluate(gameState);
    gameState.freeRandom();
    double randValue = this->right->evaluate(gameState);

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
            if (left->evaluate(gameState) != 0)
                return this->right->simulate(gameState);
            return changes;
        default:
            return VariableChanges();
    }
}

double OperandNode::evaluate(GameState& gameState, ValueType valType) const{
    double left = 0;
    double right = 0;
    if (this->left  != nullptr) left  = this->left->evaluate(gameState, valType);
    if (this->right != nullptr) right = this->right->evaluate(gameState, valType);

    switch(this->oper){
    case Operand::Add:
        return left + right;
        break;
    case Operand::Subtract:
        return left - right;
        break;
    case Operand::Multiply:
        return left * right;
        break;
    case Operand::Divide:
        return left / right;
        break;
    case Operand::Neg:
        return -right;
    case Operand::Assign:
        return right;
    case Operand::AddAssign:
        return left + right;
    case Operand::SubAssign:
        return left - right;
    case Operand::MulAssign:
        return left * right;
    case Operand::DivAssign:
        return left / right;
    case Operand::Equal:
        if (left == right) return 1;
        return 0;
    case Operand::NotEqual:
        if (left != right) return 0;
        return 1;
    case Operand::Less:
        if (left< right) return 1;
        return 0;
    case Operand::Greater:
        if (left > right) return 1;
        return 0;
    case Operand::LessOrEqual:
        if (left <= right) return 1;
        return 0;
    case Operand::GreaterOrEqual:
        if (left >= right) return 1;
        return 0;
    case Operand::Modulo:
        return (int)left % (int)right;
    case Operand::Power:
        return pow(left, right);
    case Operand::And:
        return left != 0 && right != 0;
    case Operand::Or:
        return left != 0 || right != 0;
    case Operand::Not:
        if (right != 0) return 0;
        return 1;
    case Operand::Abs:
        return right > 0 ? right : -right;
    case Operand::Min:
        return right > left ? left : right;
    case Operand::Max:
        return right < left ? left : right;
    case Operand::If:
        if (left != 0)
            return right;
        return 0;
    default:
        return 0;
    }
}

std::vector<DisplayLine> OperandNode::insight(GameState& gameState, int level){
    DisplayLine l;
    return { };
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

RangeObject OperandNode::getRangeObject(){
    RangeObject l = left->getRangeObject();
    RangeObject r = right->getRangeObject();
    l.combine(r, oper); return l;
};

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
    if (leftRD->getType() == NodeType::Variable && rightRD->getType() == NodeType::Variable && (oper == Operand::Add || oper == Operand::Subtract)){
        VariableFlags flags;
        return std::make_unique<VariableNode>("_R", flags);
    }
    return std::make_unique<OperandNode>(oper, std::move(leftRD), std::move(rightRD));
}

std::unique_ptr<Node> OperandNode::clone() const{
    return std::make_unique<OperandNode>(this->oper, this->left ? this->left->clone() : nullptr, this->right ? this->right->clone() : nullptr);
}

std::unique_ptr<Node> OperandNode::getPacketExpression(GameState& gameState, bool base) const{
    if (isAssignment(oper)){
        if (oper == Operand::Assign)
            return this->right->getPacketExpression(gameState, false);
        Operand operand;
        if (oper == Operand::AddAssign)
            operand = Operand::Add;
        if (oper == Operand::SubAssign)
            operand = Operand::Subtract;
        if (oper == Operand::MulAssign)
            operand = Operand::Multiply;
        if (oper == Operand::DivAssign)
            operand = Operand::Divide;

        return std::make_unique<OperandNode>(operand, this->left->getPacketExpression(gameState, false), this->right->getPacketExpression(gameState, false));
    }
    if (base) return nullptr;
    return std::make_unique<OperandNode>(oper, this->left->getPacketExpression(gameState, false), this->right->getPacketExpression(gameState, false));
}

Node* OperandNode::normalize(bool negate) {
    LOG("expressiontree.cpp normalize(negate=" << negate << ") fun beg");
    if (oper == Operand::Not){
        LOG("expressiontree.cpp normalize(negate=" << negate << ") oper is not");
        if (right == nullptr) {
            LOG("right err 1");
            return nullptr;
        }
        Node* childPtr = this->right.release();
        return childPtr->normalize(!negate);
    }

    if (negate && isComparision(oper)){
        oper = getInverseOperand(oper);
        LOG("expressiontree.cpp normalize(negate=" << negate << ") oper inverted");
    }
    else{
        if (isLogicalGate(oper) && negate){
            oper = oper == Operand::And ? Operand::Or : Operand::And;
        }
        if (left) {
            Node* newLeft = left->normalize(false);
            if (newLeft != left.get()) {
                left.release();
                left.reset(newLeft);
            }
        }

        if (right) {
            Node* newRight = right->normalize(false);
            if (newRight != right.get()) {
                right.release();
                right.reset(newRight);
            }
        }
        LOG("expressiontree.cpp normalize(negate=" << negate << ") fun standart non negate");
    }

    LOG("expressiontree.cpp normalize(negate=" << negate << ") fun ret");
    return this;
}

double OperandNode::getProgress(GameState& gameState, int& weight) const{
    double res = evaluate(gameState);
    printOperand(oper);
    if (res != 0){
        weight = 1;
        return 1;
    }

    // expression is false
    if (containEnumLikeVariable()){
        weight = 1;
        return 0;
    }
    if (isComparision(oper) && oper != Operand::NotEqual){
        weight = 1;
        if (!left || !right) return 0;
        double rightEval = right->evaluate(gameState);
        double defDistance = std::abs(left->evaluate(gameState, ValueType::Default) - rightEval);
        double currDistance = std::abs(left->evaluate(gameState) - rightEval);
        if (defDistance == 0) return 0;
        if (currDistance == 0) return 0.99;
        return std::max(0.,  1-(currDistance/defDistance));
    }
    if (oper == Operand::Or){
        weight = 1;
        int tmp;
        double leftProg  = left  ? left->getProgress(gameState, tmp)  : 0;
        double rightProg = right ? right->getProgress(gameState, tmp) : 0;
        return std::max(leftProg, rightProg);
    }
    if (oper == Operand::And){
        int leftWeight  = 0; int rightWeight  = 0;
        double leftProg = 0; double rightProg = 0;
        if (left)  leftProg  = left->getProgress(gameState, leftWeight);
        if (right) rightProg = right->getProgress(gameState, rightWeight);
        
        weight = leftWeight + rightWeight;
        if (weight == 0) return 0;
        return (leftProg*leftWeight + rightProg*rightWeight)/(leftWeight+rightWeight);
    }
    return 0;
}