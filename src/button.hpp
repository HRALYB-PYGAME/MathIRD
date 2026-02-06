#ifndef button_hpp
#define button_hpp
#include "term.hpp"

class GameState;

class Button : public Insightable{
private:
    std::string name;
    std::vector<std::unique_ptr<Term>> terms;
    std::vector<ConditionalDisplayLine> display;
    ButtonPosition position;
public:
    Button(std::string name): name(name) {};

    // Display
    std::string getDisplay(GameState& gameState);
    void setDisplay(std::unique_ptr<Node> condition, std::string t);

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;

    // Term
    void addTerm(std::unique_ptr<Term>);
    const std::vector<std::unique_ptr<Term>>& getTerms() {return terms;};
    const std::vector<Expression> getExpressions(GameState& gameState) const;

    // Name
    std::string& getName() {return name;};
    void setName(const std::string& name) {this->name = name;};

    // Position
    ButtonPosition& getPosition() {return position;};
    void setPosition(ButtonPosition pos) {position = pos;};
    void setPosition(int row, int col) {position = {row, col};};

    // State
    bool isUnlocked(GameState& gameState);
    bool isActive(GameState& gameState);
    bool isUnblocked(GameState& gameState);

    VariableChanges simulate(GameState& gameState);
};

#endif