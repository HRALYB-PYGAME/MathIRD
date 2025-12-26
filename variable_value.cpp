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