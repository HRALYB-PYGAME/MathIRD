#include "expressiontree.hpp"
#include "game_state.hpp"
#include "insight.hpp"

std::set<std::string> VariableNode::getDependencies() const{
    std::set<std::string> deps;
    if (this->isSoft()) return deps;
    deps.insert(this->var);
    return deps;
}

std::set<std::string> VariableNode::getInputs(bool root) const{
    std::set<std::string> set;
    if (!root){
        set.insert(var);
    }
    return set;
}

std::set<std::string> VariableNode::getOutputs(bool root) const{
    std::set<std::string> set;
    if (!root){
        set.insert(this->var);
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

std::set<std::string> VariableNode::getBlockers(bool root) const{
    std::set<std::string> set;
    if (!isFluid()) set.insert(var);
    return set;
}

bool VariableNode::isConstant(GameState& gameState){
    return !gameState.isVariableUnlocked(this->var);
}

bool VariableNode::isConstantValue(GameState& gameState, double val){
    if (this->isConstant(gameState) && this->evaluate(gameState) == val)
        return true;
    return false;
}

bool VariableNode::containEnumLikeVariable() const{
    VariableType type = Defs::getVariable(var)->getType();
    if (type == VariableType::Boolean || type == VariableType::Enum)
        return true;
    return false;
}

double VariableNode::evaluate(GameState& gameState, ValueType valType) const{
    if (isReal()) return gameState.getRealValue(this->var);
    if (valType == ValueType::Real) return gameState.getRealValue(this->var);
    if (valType == ValueType::Default) return Defs::getVariable(var)->getDefaultValue();
    return gameState.getVarValue(this->var);
}

std::vector<DisplayLine> VariableNode::insight(GameState& gameState, [[maybe_unused]] int level){
    return { };
    std::string name = this->var;
    DisplayChunk c(name, DisplayType::Text);
    DisplayChunk h(name, DisplayType::Var);
    Variable* var = Defs::getVariable(name);
    if (var != nullptr) c.setLink(var);
    c.setHover({ h });
    return { DisplayLine({ c }) };
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

std::unique_ptr<Node> VariableNode::getRandomDistribution() { 
    if (this->var == "_R" or this->var == "_NR"){
        VariableFlags flags;
        return std::make_unique<VariableNode>("_R", flags);
    }
    return std::make_unique<GeneralConstantNode>(); 
}

std::unique_ptr<Node> VariableNode::clone() const{
    return std::make_unique<VariableNode>(this->var, this->flags);
}

std::unique_ptr<Node> VariableNode::getPacketExpression(GameState& gameState, bool base) const {
    if (base) return nullptr;
    if (isConstant()) return std::make_unique<ConstantNode>(this->evaluate(gameState));
    return std::make_unique<VariableNode>(var, flags);
};

double VariableNode::getProgress(GameState& gameState, int& weight) const{
    weight = 1;
    return evaluate(gameState) == 0 ? 0 : 1;
}