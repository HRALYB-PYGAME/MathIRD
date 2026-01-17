#ifndef insight_hpp
#define insight_hpp
#include <string>
#include "expressiontree.hpp"
#include "insightable.hpp"

std::vector<DisplayLine> addInsight      (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayLine> subtractInsight (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayLine> multiplyInsight (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayLine> divideInsight   (Node& left, Node& right, GameState& gameState, int level);

#endif