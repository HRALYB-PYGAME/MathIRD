#include "insightable.hpp"
#include "game_state.hpp"

std::string DisplayChunk::getDisplay(GameState& gameState) {
    if (type == DisplayType::Text) return text;
    return formatDouble(gameState.getVarValue(text)) + "(" + std::to_string((int)gameState.getRealValue(text)) + ")";
};

void DisplayChunk::printChunk(){
    switch(type){
        case DisplayType::Text:
            std::cout << "Text (" << text << ")\n";
            break;
        case DisplayType::Var:
            std::cout << "Var (" << text << ")\n";
            break;
        case DisplayType::NewLine:
            std::cout << "NewLine\n";
            break;
        case DisplayType::Indent:
            std::cout << "Indent (" << scalar << ")\n";
            break;
        case DisplayType::WordGap:
            std::cout << "WordGap (" << scalar << ")\n";
            break;
    }
}

void DisplayLine::printLine(){
    if (chunks.empty()){
        std::cout << "Line is empty\n";
        return;
    }
    for(auto& chunk : chunks){
        chunk.printChunk();
    }
}

void printLines(std::vector<DisplayLine>& lines){
    for(auto& line : lines){
        line.printLine();
    }
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

void DisplayLine::appendIndentChunk(double mul){
    DisplayChunk chunk(DisplayType::Indent, mul);
    chunks.insert(chunks.end(), chunk);
}

void DisplayLine::appendWordGapChunk(double mul){
    DisplayChunk chunk(DisplayType::WordGap, mul);
    chunks.insert(chunks.end(), chunk);
}

void DisplayLine::appendNewLineChunk(){
    DisplayChunk chunk("", DisplayType::NewLine);
    chunks.insert(chunks.end(), chunk);
}

void DisplayLine::appendLines(std::vector<DisplayLine> lines){
    for(auto& line : lines){
        appendChunks(line.chunks);
        appendNewLineChunk();
    }
}

ConditionalDisplayLine::ConditionalDisplayLine(DisplayLine displayLine, std::unique_ptr<Node> condition):
    displayLine(displayLine), condition(std::move(condition)) {};

bool ConditionalDisplayLine::isConditionTrue(GameState& gameState){
    return condition->evaluate(gameState) != 0;
}