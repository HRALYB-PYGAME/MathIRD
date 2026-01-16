#include "variable.hpp"
#include <cmath>
#include <algorithm>
#include "expressiontree.hpp"
#include "term.hpp"

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

Variable::Variable(std::string name, ScoreParams scoreParams, std::unique_ptr<Node> unlockCondition, VariableValue defaultValue)
    : name(std::move(name)), 
      scoreParams(scoreParams), 
      unlockCondition(std::move(unlockCondition)), 
      defaultValue(defaultValue) 
{
    
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
