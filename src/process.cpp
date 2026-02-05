#include "process.hpp"

double Process::getInterval(GameState& gameState){
    return interval->evaluate(gameState);
}

std::vector<DisplayLine> Process::insight(GameState& gameState, int level){
    return {};
}