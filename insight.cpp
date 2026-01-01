#include "insight.hpp"
using namespace std::string_literals;

std::string divideInsight(Node& left, Node& right, GameState& gameState, int level){
    double leftValue = left.evaluate(gameState).getAsDouble();
    double rightValue = right.evaluate(gameState).getAsDouble();
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);

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

std::string addInsight   (Node& left, Node& right, GameState& gameState, int level){
    std::string leftInsight = left.insight(gameState, level+1);
    std::string rightInsight = right.insight(gameState, level+1);
    if (level <= 1)
        return "sum of "s + REVERSE + leftInsight + RESET + " and " + REVERSE + rightInsight + RESET;
    return "(" + leftInsight + " plus " + rightInsight + ")";
}