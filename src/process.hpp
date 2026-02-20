#ifndef process_hpp
#define process_hpp

#include "term.hpp"

class GameState;

class Process : public Insightable{
private:
    std::unique_ptr<Node> startCondition;
    std::unique_ptr<Node> endCondition;
    std::vector<std::unique_ptr<Term>> terms;

    std::unique_ptr<Node> interval;

    std::string name;

public:
    Process(std::string name): name(name) {};

    std::string getName() {return name;};
    double getInterval(GameState& gameState) const;

    bool isStartConditionMet(GameState& gameState) const;
    bool isEndConditionMet(GameState& gameState) const;
    
    bool isLocked(GameState& gameState) const override;
    bool isActive(GameState& gameState) const override;
    bool isBlocked(GameState& gameState) const override;

    void setStartCondition(std::unique_ptr<Node> startCondition);
    void setEndCondition(std::unique_ptr<Node> endCondition);
    void setInterval(std::unique_ptr<Node> interval);

    void addTerm(std::unique_ptr<Term>);
    Term* getTerm(std::string name) const override {
        for(auto& term : terms){
            if (term->getName() == name)
                return term.get();
        }
        return nullptr;
    };
    void updateTermsSets() const{
        for(auto& term : terms){
            term->updateSets();
        }
    };
    const std::vector<std::unique_ptr<Term>>& getTerms() {return terms;};
    const std::vector<Expression> getExpressions(GameState& gameState) const;

    std::vector<DisplayLine> insight(GameState& gameState, int level) override;
    std::set<std::string> getInputs(bool root, std::string function="") const override;

    void bind();
};

#endif