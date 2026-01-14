#ifndef button_hpp
#define button_hpp
#include "term.hpp"
#include "insightable.hpp"

class Button : Insightable{
private:
    std::string name;
    std::vector<Term> terms;
    std::vector<DisplayChunk> display;
public:
    std::string getDisplay(GameState& gameState);
    void setDisplay(std::string t);

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    // simulate
    // insight
    // isUnlocked
    // 
};

#endif