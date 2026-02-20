#include "expressiontree.hpp"
#include "game_state.hpp"
#include <unordered_map>

std::unique_ptr<Node> FunctionNode::clone() const{
    return std::make_unique<FunctionNode>(this->name, this->args, this->target);
}

std::set<std::string> FunctionNode::getDependencies() const{
    return {};
}

std::set<std::string> FunctionNode::getInputs(bool root, std::string function) const{
    LOG("getting inputs of " << this->name << " function with "<< args.size() << " arguments");
    if (!target) return {};
    LOG("why am i here if target is null??");
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
    auto it = functions.find(name);
    if (it == functions.end()) return nullptr;
    //if (it->second.evaluatedBefore)
    //    return std::make_unique<ConstantNode>(this->evaluate(gameState));
    return std::make_unique<FunctionNode>(this->name, this->args, this->target);
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
    auto it = functions.find(name); 
    target = nullptr;
    if (it == functions.end()){
        LOG("function doesnt exist");
        return; // function doesnt exist
    }
    if (args.size() < it->second.minArgC || args.size() > it->second.maxArgC){
        LOG("invalid arg count");
        return; // invalid arg count
    }
    if (!it->second.bindable){
        LOG("not bindable returning");
        return;
    }
    if (args.size() == 0){
        LOG("bindables need atleast one argument");
        return; // for bindable atleast one char
    }
    target = Defs::getTarget(args[0]);
    if (target == nullptr){
        LOG("target is null");
        return; // invalid target
    }
}

double FunctionNode::evaluate(GameState& gameState, ValueType valType) const{
    if (name == "isUnlocked"){
        if (!target) return 0.0;
        return target->isLocked(gameState) ? 0.0 : 1.0;
    }
    if (name == "packetsInTransit"){
        if (args.size() == 0){
            LOG("packetsInTransit evaluate gettingPacketsCount from gameState");
            LOG("there are " << gameState.getPackets().size() << " packets");
            return gameState.getPacketsCount();
        }
        return gameState.getPacketsCount(args[0]);
    }
    return 0.0;
}