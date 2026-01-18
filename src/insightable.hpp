#ifndef insightable_hpp
#define insightable_hpp
#include <string>
#include <memory>
#include "utils.hpp"
#include <vector>
#include <iostream>

class GameState;
class Insightable;

enum class Alignment{
    Left,
    Center,
    Right
};

enum class DisplayType{
    Text,
    Var,
    NewLine,
    Indent,
    WordGap
};

struct DisplayChunk{
    std::string text;
    DisplayType type;
    std::vector<DisplayChunk> hover;
    Insightable* link;
    double scalar;

    DisplayChunk(std::string text, std::vector<DisplayChunk> hover = {}, Insightable* link = nullptr)
        : text(std::move(text)), type(DisplayType::Text), hover(std::move(hover)), link(link) {};
    DisplayChunk(std::string text, DisplayType type, std::vector<DisplayChunk> hover = {}, Insightable* link = nullptr)
        : text(std::move(text)), type(type), hover(std::move(hover)), link(link) {};
    DisplayChunk(DisplayType type, double scalar)
        : type(type), scalar(scalar) {};
    std::string getDisplay(GameState& gameState);

    void setHover(std::vector<DisplayChunk> hover) { this->hover = std::move(hover); };
    void setLink(Insightable* link){ this->link = link; };

    void printChunk();
};

struct DisplayLine{
    DisplayLine() {};
    DisplayLine(std::vector<DisplayChunk> chunks): chunks(std::move(chunks)) {}

    std::vector<DisplayChunk> chunks;
    Alignment alignment = Alignment::Left;
    float indent = 0.0f;

    void appendChunks(std::vector<DisplayChunk>& chunks);

    void appendChunk(DisplayChunk& chunk);

    void appendTextChunk(std::string name);
    void appendVarChunk(std::string name);
    void appendIndentChunk(double mul);
    void appendWordGapChunk(double mul);
    void appendNewLineChunk();

    void appendLines(std::vector<DisplayLine> lines);

    void printLine();
};

void printLines(std::vector<DisplayLine>& lines);

class Insightable{
public:
    virtual std::vector<DisplayLine> insight(GameState& gameState, int level) = 0;
};

#endif