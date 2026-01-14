#include "insightable.hpp"
#include "game_state.hpp"

std::string DisplayChunk::getDisplay(GameState& gameState) {
    if (type == DisplayType::Text) return val;
    return formatDouble(gameState.getVarValueAsDouble(val));
};

void DisplayLine::appendChunks(std::vector<DisplayChunk>& chunks){
    this->chunks.insert(this->chunks.end(), chunks.begin(), chunks.end());
}

void DisplayLine::appendChunk(DisplayChunk& chunk){
    this->chunks.insert(this->chunks.end(), chunk);
}

void DisplayLine::appendTextChunk(std::string name){
    DisplayChunk chunk(name, DisplayType::Text);
    chunks.insert(chunks.end(), chunk);
}

void DisplayLine::appendVarChunk(std::string name){
    DisplayChunk chunk(name, DisplayType::Var);
    chunks.insert(chunks.end(), chunk);
}

void DisplayLine::appendLines(std::vector<DisplayLine> lines){
    for(auto& line : lines){
        appendChunks(line.chunks);
    }
}