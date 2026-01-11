#ifndef button_hpp
#define button_hpp
#include "term.hpp"

enum class DisplayType{
    Text,
    Var
};

class DisplayChunk{
private:
    std::string val;
    DisplayType type;
public:
    DisplayChunk(std::string val, DisplayType type): val(val), type(type) {};
    std::string getDisplay(GameState& gameState) {
        if (type == DisplayType::Text) return val;
        return formatDouble(gameState.getVarValueAsDouble(val));
    };
};

class Button{
private:
    std::string name;
    std::vector<Term> terms;
    std::vector<DisplayChunk> display;
public:
    std::string getDisplay(GameState& gameState);
    void setDisplay(std::string t);
    // simulate
    // insight
    // isUnlocked
    // 
};

#endif