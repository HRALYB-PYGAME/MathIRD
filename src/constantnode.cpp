#include "expressiontree.hpp"
#include "game_state.hpp"
#include "insight.hpp"

bool ConstantNode::isConstantValue([[maybe_unused]] GameState& gameState, double val){
    if (this->val == val) return true;
    return false;
}

double ConstantNode::evaluate([[maybe_unused]] GameState& gameState, ValueType valType) const {
    return this->val;
}

std::vector<DisplayLine> ConstantNode::insight(GameState& gameState, [[maybe_unused]] int level){
    return { };
    double val = this->evaluate(gameState);
    DisplayChunk c(formatDouble(val), DisplayType::Text);
    return { DisplayLine({ c }) };
};  

std::vector<DisplayLine> GeneralConstantNode::insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level){
    DisplayChunk c("C", DisplayType::Text);
    return { DisplayLine({ c }) };
}

std::vector<DisplayLine> ConstantNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    line.appendTextChunk(formatDouble(this->val));
    return { line };
}

std::vector<DisplayLine> GeneralConstantNode::arithmeticalInsight(GameState& gameState, int level){
    DisplayLine line;
    line.appendTextChunk("C");
    return { line };
}

std::unique_ptr<Node> ConstantNode::getRandomDistribution() { 
    return std::make_unique<GeneralConstantNode>(); 
}

std::unique_ptr<Node> ConstantNode::clone() const{
    return std::make_unique<ConstantNode>(this->val);
}

Node* ConstantNode::normalize(bool negate) {
    if (negate){
        val = val != 0 ? 0 : 1; 
    }
    return this;
}

double ConstantNode::getProgress(GameState& gameState, int& weight) const{
    weight = 1;
    return val == 0 ? 0 : 1;
}