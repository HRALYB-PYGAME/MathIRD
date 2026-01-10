#include "insight.hpp"
using namespace std::string_literals;


std::string addInsight      (Node& left, Node& right, GameState& gameState, int level){
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);
    if (left.isConstant(gameState) && left.evaluate(gameState).getAsDouble() == 0)
        return rightInsight;
    if (right.isConstant(gameState) && right.evaluate(gameState).getAsDouble() == 0)
        return leftInsight;
    if (left.isConstant(gameState) && right.isConstant(gameState))
        return formatDouble(left.evaluate(gameState).getAsDouble() + right.evaluate(gameState).getAsDouble());
    if (level <= 1)
        return "the sum of "s + REVERSE + leftInsight + RESET + " and " + REVERSE + rightInsight + RESET;
    return "("s + leftInsight + " plus " + rightInsight + ")";
}

std::string subtractInsight (Node& left, Node& right, GameState& gameState, int level){
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);
    if (left.isConstant(gameState) && right.isConstant(gameState))
        return formatDouble(left.evaluate(gameState).getAsDouble() - right.evaluate(gameState).getAsDouble());
    if (level <= 1)
        return REVERSE + leftInsight + RESET + " decreased by " + REVERSE + rightInsight + RESET;
    return "(" + leftInsight + " minus " + rightInsight + ")";
}

std::string multiplyInsight (Node& left, Node& right, GameState& gameState, int level){
    double rightValue = right.evaluate(gameState).getAsDouble();
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);
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
    return "(" + leftInsight + " times " + rightInsight + ")";
}

std::string divideInsight   (Node& left, Node& right, GameState& gameState, int level){
    double leftValue = left.evaluate(gameState).getAsDouble();
    double rightValue = right.evaluate(gameState).getAsDouble();
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);
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
    return "(" + leftInsight + "/" + rightInsight + ")";
}