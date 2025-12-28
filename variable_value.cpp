#include "variable_value.hpp"

int VariableValue::getInt(){
    return std::get<int>(this->value);
}
double VariableValue::getDouble(){
    return std::get<double>(this->value);
}
bool VariableValue::getBool(){
    return std::get<bool>(this->value);

}
double VariableValue::getAsDouble(){
    if (this->type == Type::INT)
        return (double)std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1.0 : 0.0;
    return std::get<double>(value);
}
double VariableValue::getAsInt(){
    if (this->type == Type::INT)
        return std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1 : 0;
    return (int)std::get<double>(value);
}
Type VariableValue::getType(){
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

VariableValue VariableValue::add(VariableValue& other){
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() + other.getAsDouble());
    return VariableValue(this->getAsInt() + other.getAsInt());
}

VariableValue VariableValue::subtract(VariableValue& other){
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() - other.getAsDouble());
    return VariableValue(this->getAsInt() - other.getAsInt());
}

VariableValue VariableValue::multiply(VariableValue& other){
    if (this->type == Type::DOUBLE || other.type == Type::DOUBLE)
        return VariableValue(this->getAsDouble() * other.getAsDouble());
    return VariableValue(this->getAsInt() * other.getAsInt());
}

VariableValue VariableValue::divide(VariableValue& other){
    if (other.getAsDouble() == 0) return VariableValue(0.0);
    return VariableValue(this->getAsDouble() / other.getAsDouble());
}