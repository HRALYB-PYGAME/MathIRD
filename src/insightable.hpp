#ifndef insightable_hpp
#define insightable_hpp
#include <string>
#include <memory>
#include "utils.hpp"
#include <vector>
#include <iostream>
#include <set>

class GameState;
class Insightable;
class Node;
class Term;

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
    std::vector<DisplayChunk> chunks;
    Alignment alignment = Alignment::Left;

    DisplayLine() {};
    DisplayLine(std::vector<DisplayChunk> chunks): chunks(std::move(chunks)) {};
    DisplayLine(DisplayChunk chunk): chunks({ chunk }) {};

    float indent = 0.0f;

    void appendChunks(std::vector<DisplayChunk>& chunks);

    void appendChunk(DisplayChunk& chunk);

    void appendTextChunk(std::string name);
    void appendVarChunk(std::string name);
    void appendIndentChunk(double mul);
    void appendWordGapChunk(double mul);
    void appendNewLineChunk();
    void appendNewLineTextChunk(std::string name) {appendNewLineChunk(); appendTextChunk(name);};

    void appendLines(std::vector<DisplayLine> lines);

    void clear() {chunks.clear();};

    void printLine();
};

struct ConditionalDisplayLine{
    DisplayLine displayLine;
    std::unique_ptr<Node> condition;

    ConditionalDisplayLine(DisplayLine displayLine, std::unique_ptr<Node> condition);

    bool isConditionTrue(GameState& gameState);
};

void printLines(std::vector<DisplayLine>& lines);

enum InsightableState{
    Locked,
    Inactive,
    Blocked,
    Unblocked
};

class Insightable{
public:
    virtual std::vector<DisplayLine> insight(GameState& gameState, int level) = 0;

    virtual std::set<std::string> getInputs(bool root, std::string function = "") const { return {}; };

    virtual bool isLocked(GameState& gameState) const { return false; };
    virtual bool isActive(GameState& gameState) const { return false; };
    virtual bool isBlocked(GameState& gameState) const { return false; };

    InsightableState getState(GameState& gameState) const{
        if (isLocked(gameState)) return InsightableState::Locked;
        if (!isActive(gameState)) return InsightableState::Inactive;
        if (isBlocked(gameState)) return InsightableState::Blocked;
        return InsightableState::Unblocked;
    };

    virtual Term* getTerm(std::string name) const {return nullptr;};
};

#endif