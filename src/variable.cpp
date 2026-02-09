#include "variable.hpp"
#include "game_state.hpp"
#include <cmath>
#include <algorithm>

double ScoreParams::getScore(double val){
    if (polarity == Polarity::Neutral)
        return 0.0;
        
    val -= offset;

    if (polarity == Polarity::Inverted)
        val = -val;
    if (polarity == Polarity::Absolute)
        val = abs(val);

    if (val <= 0) return 0.f;

    double logValue = log(val)/log(knee);

    if (logValue <= 0) return 0.f;
    return logValue*100;
}

ScoreParams getScoreParams(double knee, double offset, Polarity polarity) {
    return {knee, offset, polarity};
}

Variable::Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, double defaultValue, VariableType type)
    : name(std::move(name)), 
      scoreParams(scoreParams), 
      unlockCondition(std::move(unlockCondition)), 
      defaultValue(defaultValue),
      homeButton(nullptr),
      type(type)
{
    
}

double Variable::constrain(double value) const{
    switch (type){
        case VariableType::Boolean:
            return value > 0.5 ? 1 : 0;
        case VariableType::Double:
            return value;
        case VariableType::Enum:
            return std::clamp((int)value, 0, (int)getStateCount()-1);
        case VariableType::Int:
            return (int)value;
        case VariableType::Percentage:
            return std::clamp(value, 0.0, 1.0);
    }
    return value;
}

std::vector<DisplayLine> Variable::insight(GameState& gameState, int level){
    return {};
}

void Variable::addTermAsDependency(Term* term){
    asDependencyInTerms.insert(term);
}

void Variable::addTermAsInput(Term* term){
    asInputInTerms.insert(term);
}

void Variable::addTermAsOutput(Term* term){
    asOutputInTerms.insert(term);
}

void Variable::addTermAsBlocker(Term* term){
    asBlockerInTerms.insert(term);
}

void Variable::addButtonAsDisplay(Button* button){
    displayedAtButtons.insert(button);
}

void Variable::printDependencies(){
    std::cout << "Dependencies (" << asDependencyInTerms.size() << "):\n";
    for(auto& depen : asDependencyInTerms){
        std::cout << depen->getName() << std::endl;
    }
    std::cout << "\nInputs (" << asInputInTerms.size() << "):\n";
    for(auto& input : asInputInTerms){
        std::cout << input->getName() << std::endl;
    }
    std::cout << "\nOutputs (" << asOutputInTerms.size() << "):\n";
    for(auto& output : asOutputInTerms){
        std::cout << output->getName() << std::endl;
    }
}
