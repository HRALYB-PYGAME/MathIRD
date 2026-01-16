#include "insight.hpp"
using namespace std::string_literals;


std::vector<DisplayChunk> addInsight (Node& left, Node& right, GameState& gameState, int level){
    return {};
    // std::vector<DisplayChunk> leftInsight = left.insight(gameState, level+1);
    // std::vector<DisplayChunk> rightInsight = right.insight(gameState, level+1);
    // std::vector<DisplayChunk> s;
    // if (left.isConstantValue(gameState, 0))
    //     return rightInsight;
    // if (right.isConstantValue(gameState, 0))
    //     return leftInsight;
    // if (left.isConstant(gameState) && right.isConstant(gameState))
    //     return { DisplayChunk(formatDouble(left.evaluate(gameState).getAsDouble() + right.evaluate(gameState).getAsDouble()), DisplayType::Text) };
    // if (level <= 1){
    //     // s.appendTextChunk("the sum of ");
    //     // appendChunks(s, leftInsight);
    //     // appendTextChunk(s, " and ");
    //     // appendChunks(s, rightInsight);
    //     return s;
    // }
    // // appendTextChunk(s, "(");
    // // appendChunks(s, leftInsight);
    // // appendTextChunk(s, " plus ");
    // // appendChunks(s, rightInsight);
    // // appendTextChunk(s, ")");
    // return s;
}

std::vector<DisplayChunk> subtractInsight (Node& left, Node& right, GameState& gameState, int level){
    return {};
    /*std::vector<DisplayChunk> leftInsight = left.insight(gameState, level+1);
    std::vector<DisplayChunk> rightInsight = right.insight(gameState, level+1);
    if (left.isConstantValue(gameState, 0))
        return rightInsight;
    if (right.isConstantValue(gameState, 0))
        return leftInsight;
    if (left.isConstant(gameState) && right.isConstant(gameState))
        return formatDouble(left.evaluate(gameState).getAsDouble() - right.evaluate(gameState).getAsDouble());
    if (level <= 1)
        return REVERSE + leftInsight + RESET + " decreased by " + REVERSE + rightInsight + RESET;
    return "(" + leftInsight + " minus " + rightInsight + ")";*/
}

std::vector<DisplayChunk> multiplyInsight (Node& left, Node& right, GameState& gameState, int level){
    return {};
    /*double rightValue = right.evaluate(gameState).getAsDouble();
    std::vector<DisplayChunk> leftInsight = left.insight(gameState, level+1);
    std::vector<DisplayChunk> rightInsight = right.insight(gameState, level+1);
    if (left.isConstantValue(gameState, 1))
        return rightInsight;
    if (right.isConstantValue(gameState, 1))
        return leftInsight;
    if (left.isConstant(gameState) && right.isConstant(gameState))
        return formatDouble(left.evaluate(gameState).getAsDouble() * right.evaluate(gameState).getAsDouble());
    if (level <= 1){
        if (right.getType() == NodeType::Constant){
            if (rightValue == 2.0)
                return "double of " + leftInsight;
            if (rightValue == 3.0)
                return "triple of " + leftInsight;
        }
        return "the product of "s + REVERSE + leftInsight + RESET + " and " + REVERSE + rightInsight + RESET;
    }
    return "(" + leftInsight + " times " + rightInsight + ")";*/
}

std::vector<DisplayChunk> divideInsight (Node& left, Node& right, GameState& gameState, int level){
    return {};
    /*double leftValue = left.evaluate(gameState).getAsDouble();
    double rightValue = right.evaluate(gameState).getAsDouble();
    std::vector<DisplayChunk> leftInsight = left.insight(gameState, level+1);
    std::vector<DisplayChunk> rightInsight = right.insight(gameState, level+1);
    if (left.isConstant(gameState))
        return rightInsight;
    if (right.isConstant(gameState))
        return leftInsight;
    if (left.isConstant(gameState) && right.isConstant(gameState))
        return formatDouble(left.evaluate(gameState).getAsDouble() / right.evaluate(gameState).getAsDouble());
    if (level <= 1){
        if (left.getType() == NodeType::Constant){
            if (leftValue == 1)
                return "the reciprocal of " + rightInsight;
        }
        if (right.getType() == NodeType::Constant){
            std::vector<std::string> fractions = {"a half", "a third", "a quarter", "a fifth", "a sixth", "a seventh", "an eighth", "a ninth", "a tenth"};
            double intpart;
            if (modf(rightValue, &intpart) == 0.0 && rightValue>=2 && rightValue<=10)
                return fractions[rightValue-2] + " of " + leftInsight;
        }
        return leftInsight + " divided by " + rightInsight;
    }
    return "(" + leftInsight + "/" + rightInsight + ")";*/
}