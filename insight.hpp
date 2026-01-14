#ifndef insight_hpp
#define insight_hpp
#include <string>
#include "expressiontree.hpp"
#include "insightable.hpp"

std::vector<DisplayChunk> addInsight      (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayChunk> subtractInsight (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayChunk> multiplyInsight (Node& left, Node& right, GameState& gameState, int level);
std::vector<DisplayChunk> divideInsight   (Node& left, Node& right, GameState& gameState, int level);

#endif