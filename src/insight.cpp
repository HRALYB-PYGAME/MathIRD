#include "insight.hpp"
using namespace std::string_literals;

std::vector<DisplayLine> addInsight (Node& left, Node& right, GameState& gameState, int level){
    std::vector<DisplayLine> leftInsight;
    std::vector<DisplayLine> rightInsight;
    if (level == 0){
        leftInsight = left.insight(gameState, level+1);
        rightInsight = right.insight(gameState, level+1);
    }
    else{
        leftInsight = left.arithmeticalInsight(gameState, 0);
        rightInsight = right.arithmeticalInsight(gameState, 0);
    }

    DisplayLine line;
    if (left.isConstantValue(gameState, 0))
        return rightInsight;
    if (right.isConstantValue(gameState, 0))
        return leftInsight;
    if (left.isConstant(gameState) && right.isConstant(gameState)){
        line.appendTextChunk(formatDouble(left.evaluate(gameState).getAsDouble() + right.evaluate(gameState).getAsDouble()));
        return { line };
    }
    line.appendTextChunk("the sum of ");
    line.appendLines(leftInsight);
    line.appendTextChunk("and");
    line.appendLines(rightInsight);
    return { line };
}

std::vector<DisplayLine> subtractInsight (Node& left, Node& right, GameState& gameState, int level){
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

std::vector<DisplayLine> multiplyInsight (Node& left, Node& right, GameState& gameState, int level){
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

std::vector<DisplayLine> divideInsight (Node& left, Node& right, GameState& gameState, int level){
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