#include "variable_value.hpp"
#include "utils.hpp"
#include <iostream>
#include "game_state.hpp"

int VariableValue::getInt() const{
    return std::get<int>(this->value);
}
double VariableValue::getDouble() const{
    return std::get<double>(this->value);
}
bool VariableValue::getBool() const{
    return std::get<bool>(this->value);

}
double VariableValue::getAsDouble() const{
    if (this->type == Type::INT)
        return (double)std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1.0 : 0.0;
    return std::get<double>(value);
}
int VariableValue::getAsInt() const{
    if (this->type == Type::INT)
        return std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1 : 0;
    return (int)std::get<double>(value);
}
bool VariableValue::getAsBool() const{
    if (this->getAsDouble() == 0) return false;
    return true;
}
Type VariableValue::getType() const{
    return this->type;
}

void VariableValue::set(int v){
    this->value = v;
    this->type = Type::INT;
}
void VariableValue::set(double v){
    this->value = v;
    this->type = Type::DOUBLE;
}
void VariableValue::set(bool v){
    this->value = v;
    this->type = Type::BOOL;
}

void VariableValue::set(VariableValue v){
    switch(v.type){
    case Type::BOOL:
        this->set(v.getBool());
        break;
    case Type::DOUBLE:
        this->set(v.getDouble());
        break;
    case Type::INT:
        this->set(v.getInt());
        break;
    }
}

bool VariableValue::isGreaterThan(const VariableValue& other) const{
    if (this->getAsDouble() > other.getAsDouble()) return true;
    return false;
}

bool VariableValue::isLessThan(const VariableValue& other) const{
    if (this->getAsDouble() < other.getAsDouble()) return true;
    return false;
}

bool VariableValue::isEqualTo(const VariableValue& other) const{
    if (this->getAsDouble() == other.getAsDouble()) return true;
    return false;
}

VariableValue VariableValue::modulo(const VariableValue& other) const{
    int v1 = this->getAsInt();
    int v2 = other.getAsInt();
    return VariableValue(v1%v2);
}

VariableValue VariableValue::power(const VariableValue& other) const{
    return VariableValue(pow(this->getAsDouble(), other.getAsDouble()));
}

bool VariableValue::logAnd(const VariableValue& other) const{
    if (this->getAsBool() && other.getAsBool()) return true;
    return false;
}

bool VariableValue::logOr(const VariableValue& other) const{
    if (this->getAsBool() || other.getAsBool()) return true;
    return false;
}

bool VariableValue::logNot() const{
    if (this->getAsBool()) return false;
    return true;
}

VariableValue VariableValue::add(const VariableValue& other) const{
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() + other.getAsDouble());
    return VariableValue(this->getAsInt() + other.getAsInt());
}

VariableValue VariableValue::subtract(const VariableValue& other) const{
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() - other.getAsDouble());
    return VariableValue(this->getAsInt() - other.getAsInt());
}

VariableValue VariableValue::multiply(const VariableValue& other) const{
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() * other.getAsDouble());
    return VariableValue(this->getAsInt() * other.getAsInt());
}

VariableValue VariableValue::divide(const VariableValue& other) const{
    if (other.getAsDouble() == 0) return VariableValue(0.0);
    return VariableValue(this->getAsDouble() / other.getAsDouble());
}

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