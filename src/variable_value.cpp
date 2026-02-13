#include "variable_value.hpp"
#include <iostream>
#include "game_state.hpp"

VariableChanges VariableChanges::add(const VariableChanges& varc){
    for (auto& [var, delta] : varc.changes){
        if (this->changes.find(var) != this->changes.end()){
            this->changes.at(var).min += delta.min;
            this->changes.at(var).max += delta.max;
            this->changes.at(var).rand += delta.rand;
        }
        else{
            this->changes.insert({var, Range(delta.min, delta.max, delta.rand)});
        }
    }
    return *this;
}

VariableChanges VariableChanges::add(std::string var, double val){
    if (this->changes.find(var) != this->changes.end()){
        this->changes.at(var).min += val;
        this->changes.at(var).max += val;
        this->changes.at(var).rand += val;
    }
    else{
        this->changes.insert({var, Range(val, val, val)});
    }
    return *this;
}

VariableChanges VariableChanges::add(std::string var, double min, double max, double rand){
    LOG("variable_value.cpp add(var=" << var << ", min=" << min << ", max=" << max << ", rand=" << rand << ") FUNCTION BEG");
    if (min > max){
        double tmp = min;
        min = max;
        max = tmp;
    }
    if (this->changes.find(var) != this->changes.end()){
        this->changes.at(var).min += min;
        this->changes.at(var).max += max;
        this->changes.at(var).rand += rand;
    }
    else{
        this->changes.insert({var, Range(min, max, rand)});
    }
    LOG("variable_value.cpp add(var=" << var << ", min=" << min << ", max=" << max << ", rand=" << rand << ") CHANGES SUCCESFULL");
    return *this;
}

std::vector<DisplayLine> VariableChanges::insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level){
    std::vector<DisplayLine> lines;
    for (auto& [name, delta] : this->changes){
        DisplayLine line;
        DisplayChunk valChunk(name, DisplayType::Var);
        DisplayChunk varChunk(name, DisplayType::Text);
        varChunk.setHover({ valChunk });
        Variable* var = Defs::getVariable(name);
        if (var != nullptr) varChunk.setLink(var);
        line.appendChunk(varChunk);

        if (delta.min == delta.max){
            line.appendTextChunk(": " + formatDouble(delta.min));
        }
        else{
            line.appendTextChunk(": " + formatDouble(delta.min) + "-" + formatDouble(delta.max));
        }
        lines.push_back(line);
    }

    return lines;
}

std::string VariableChanges::print() const{
    std::string out = "";
    for (auto& [name, delta] : changes){
        out += name + " " + formatDouble(delta.rand, true);
    }
}