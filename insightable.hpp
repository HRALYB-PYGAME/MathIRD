#ifndef insightable_hpp
#define insightable_hpp
#include <string>
#include <memory>
#include "utils.hpp"
#include <vector>

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
    NewLine
};

struct DisplayChunk{
    std::string val;
    DisplayType type;
    std::vector<DisplayChunk> hover;
    Insightable* link;

    DisplayChunk(std::string val, std::vector<DisplayChunk> hover = {}, Insightable* link = nullptr)
        : val(std::move(val)), type(DisplayType::Text), hover(std::move(hover)), link(link) {};
    DisplayChunk(std::string val, DisplayType type, std::vector<DisplayChunk> hover = {}, Insightable* link = nullptr)
        : val(std::move(val)), type(type), hover(std::move(hover)), link(link) {};
    std::string getDisplay(GameState& gameState);

    void setHover(std::vector<DisplayChunk> hover) { this->hover = std::move(hover); };
    void setLink(Insightable* link){ this->link = link; };
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

    void appendLines(std::vector<DisplayLine> lines);
};

class Insightable{
public:
    virtual std::vector<DisplayLine> insight(GameState& gameState, int level) = 0;
};

#endif