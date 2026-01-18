#ifndef button_hpp
#define button_hpp
#include "term.hpp"
#include "insightable.hpp"

class Button : Insightable{
private:
    std::string name;
    std::vector<std::unique_ptr<Term>> terms;
    std::vector<DisplayChunk> display;
public:
    Button(std::string name): name(name) {};

    std::string getDisplay(GameState& gameState);
    void setDisplay(std::string t);

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    void addTerm(std::unique_ptr<Term>);

    const std::vector<std::unique_ptr<Term>>& getTerms() {return terms;};
    std::string& getName() {return name;};
    
    void setName(const std::string& name) {this->name = name;};
    bool isUnlocked(GameState& gameState);
    // simulate
    // insight
    // 
};

#endif