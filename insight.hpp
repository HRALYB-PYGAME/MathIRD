#ifndef insight_hpp
#define insight_hpp
#include <string>
#include "expressiontree.hpp"

std::string addInsight      (Node& left, Node& right, GameState& gameState, int level);
std::string subtractInsight (Node& left, Node& right, GameState& gameState, int level);
std::string multiplyInsight (Node& left, Node& right, GameState& gameState, int level);
std::string divideInsight   (Node& left, Node& right, GameState& gameState, int level);

#endif