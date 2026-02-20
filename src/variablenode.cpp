#include "expressiontree.hpp"
#include "game_state.hpp"
#include "insight.hpp"

std::set<std::string> VariableNode::getDependencies() const{
    std::set<std::string> deps;
    if (this->isSoft()) return deps;
    deps.insert(varName);
    return deps;
}

std::set<std::string> VariableNode::getInputs(bool root, std::string function) const{
    std::set<std::string> set;
    if (!root){
        set.insert(varName);
    }
    return set;
}

std::set<std::string> VariableNode::getOutputs(bool root) const{
    std::set<std::string> set;
    if (!root){
        set.insert(varName);
    }
    return set;
}

Node* VariableNode::normalize(bool negate) {
    if (!negate) return this;
    
    return new OperandNode(
        Operand::Equal, 
        std::unique_ptr<Node>(this), 
        std::make_unique<ConstantNode>(0)
    );
}

void VariableNode::bind(){
    
}

std::set<std::string> VariableNode::getBlockers(bool root) const{
    std::set<std::string> set;
    if (!isFluid()) set.insert(varName);
    return set;
}

bool VariableNode::isConstant(GameState& gameState){
    return !gameState.isVariableUnlocked(varName);
}

bool VariableNode::isConstantValue(GameState& gameState, double val){
    if (this->isConstant(gameState) && this->evaluate(gameState) == val)
        return true;
    return false;
}

bool VariableNode::containEnumLikeVariable() const{
    if (!var) return VariableType::Unkown;
    VariableType type = var->getType();
    if (type == VariableType::Boolean || type == VariableType::Enum)
        return true;
    return false;
}

double VariableNode::evaluate(GameState& gameState, ValueType valType) const{
    if (!var) return 0.0;
    if (isReal()) return gameState.getRealValue(varName);
    if (valType == ValueType::Real) return gameState.getRealValue(varName);
    if (valType == ValueType::Default) return var->getDefaultValue();
    return gameState.getVarValue(varName);
}

std::vector<DisplayLine> VariableNode::insight(GameState& gameState, [[maybe_unused]] int level){
    return { };
    DisplayChunk c(varName, DisplayType::Text);
    DisplayChunk h(varName, DisplayType::Var);
    if (var != nullptr) c.setLink(var);
    c.setHover({ h });
    return { DisplayLine({ c }) };
}

std::vector<DisplayLine> VariableNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    if (varName == "_R" || varName == "_NR"){
        line.appendTextChunk("R");
        return { line };
    }
    DisplayChunk c(varName, DisplayType::Text);
    DisplayChunk h(varName, DisplayType::Var);
    if (var != nullptr) c.setLink(var);
    c.setHover({ h });
    line.appendChunk(c);
    return { line };
}

std::unique_ptr<Node> VariableNode::getRandomDistribution() { 
    if (varName == "_R" or varName == "_NR"){
        VariableFlags flags;
        return std::make_unique<VariableNode>("_R", flags);
    }
    return std::make_unique<GeneralConstantNode>(); 
}

std::unique_ptr<Node> VariableNode::clone() const{
    return std::make_unique<VariableNode>(varName, flags, var);
}

std::unique_ptr<Node> VariableNode::getPacketExpression(GameState& gameState, bool base) const {
    if (base) return nullptr;
    if (isConstant()) return std::make_unique<ConstantNode>(this->evaluate(gameState));
    return std::make_unique<VariableNode>(varName, flags, var);
};

double VariableNode::getProgress(GameState& gameState, int& weight) const{
    weight = 1;
    return evaluate(gameState) == 0 ? 0 : 1;
}