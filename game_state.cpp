#include "game_state.hpp"

GameState::GameState(){
    variables.emplace(
        "x",
        Variable("x", 0.0, getScoreParams(100, 0, Polarity::Normal))
    );
    variables.emplace(
        "y",
        Variable("y", 5.0, getScoreParams(100, 0, Polarity::Normal))
    );
    variables.emplace(
        "s",
        Variable("s", 5.0, getScoreParams(100, 100, Polarity::Absolute))
    );
    variables.emplace(
        "mq",
        Variable("mq", 5.0, getScoreParams(25, 0, Polarity::Normal))
    );
}

double GameState::getScore(){
    double score = 0;
    for (auto& [name, var] : this->variables){
        score += var.getScore();
    }
    return score;
}

Variable* GameState::getVar(std::string name){
    auto pair = this->variables.find(name); 
    if (pair != this->variables.end()){
        return &(pair->second);
    }
    else{
        return nullptr;
    }
    
}