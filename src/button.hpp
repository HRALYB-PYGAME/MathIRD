#ifndef button_hpp
#define button_hpp
#include "term.hpp"
#include "insightable.hpp"
#include "variable_value.hpp"

struct ButtonPosition{
    int row;
    int col;
};

class Button : Insightable{
private:
    std::string name;
    std::vector<std::unique_ptr<Term>> terms;
    std::vector<DisplayChunk> display;
    ButtonPosition position;
    // bool pressedDownLeft = false;
    // bool pressedDownRight = false;
public:
    Button(std::string name): name(name) {};

    // Display
    std::string getDisplay(GameState& gameState);
    void setDisplay(std::string t);

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;

    // Term
    void addTerm(std::unique_ptr<Term>);
    const std::vector<std::unique_ptr<Term>>& getTerms() {return terms;};

    // Name
    std::string& getName() {return name;};
    void setName(const std::string& name) {this->name = name;};

    // Position
    ButtonPosition& getPosition() {return position;};
    void setPosition(ButtonPosition pos) {position = pos;};
    void setPosition(int row, int col) {position = {row, col};};

    bool isUnlocked(GameState& gameState);

    VariableChanges simulate(GameState& gameState);

    // void pressDownLeft() {pressedDownLeft = true;};
    // void pressDownRight() {pressedDownRight = true;};
    // void release() {pressedDownLeft = false; pressedDownRight = false;};
    // void releaseLeft() {pressedDownLeft = false;};
    // void releaseRight() {pressedDownRight = false;};
    // bool isPressedDownLeft() {return pressedDownLeft;};
    // bool isPressedDownRight() {return pressedDownRight;};
};

#endif