#include "expressiontree.hpp"

std::unique_ptr<Node> FunctionNode::clone() const{
    return std::make_unique<FunctionNode>(this->name, this->args);
}

std::set<std::string> FunctionNode::getDependencies() const{
    return {};
}

std::set<std::string> FunctionNode::getInputs(bool root, std::string function) const{
    if (!target) return {};
    return target->getInputs(true, this->name);
    // return input of
}

std::set<std::string> FunctionNode::getOutputs([[maybe_unused]] bool root) const{
    return {};
}

std::set<std::string> FunctionNode::getBlockers(bool root) const{
    return {};
}


double FunctionNode::getProgress(GameState& gameState, int& weight) const{
    if (evaluate(gameState) != 0) return 1.0;
    return 0.0; 
}

RangeObject FunctionNode::getRangeObject() {
    // idk
}

std::unique_ptr<Node> FunctionNode::getRandomDistribution() {
    return std::make_unique<GeneralConstantNode>();
}


std::unique_ptr<Node> FunctionNode::getPacketExpression(GameState& gameState, bool base) const {
    if (base) return nullptr;
    auto node = std::make_unique<FunctionNode>(this->name, this->args);
    node->target = this->target;
    return node;
}

std::vector<DisplayLine> FunctionNode::insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level){
    return {};
}

std::vector<DisplayLine> FunctionNode::arithmeticalInsight(GameState& gameState, int level){
    return {};
}

Node* FunctionNode::normalize(bool negate){
    if (!negate) return this;
    
    return new OperandNode(
        Operand::Equal, 
        std::unique_ptr<Node>(this), 
        std::make_unique<ConstantNode>(0)
    );
}

void FunctionNode::bind(){
    if (args.empty()) return;
    std::cout << args[0] << "\n";
    target = Defs::getTarget(args[0]);
    if (target == nullptr) std::cout << "target is null\n";
}

double FunctionNode::evaluate(GameState& gameState, ValueType valType) const{
    if (!target) return 0.0;
    if (name == "isUnlocked"){
        return target->isUnlocked(gameState) ? 1.0 : 0.0;
    }
    return 0.0;
}