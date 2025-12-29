#include "variable_value.hpp"

const int VariableValue::getInt() const{
    return std::get<int>(this->value);
}
const double VariableValue::getDouble() const{
    return std::get<double>(this->value);
}
const bool VariableValue::getBool() const{
    return std::get<bool>(this->value);

}
const double VariableValue::getAsDouble() const{
    if (this->type == Type::INT)
        return (double)std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1.0 : 0.0;
    return std::get<double>(value);
}
const double VariableValue::getAsInt() const{
    if (this->type == Type::INT)
        return std::get<int>(this->value);
    if (this->type == Type::BOOL)
        return std::get<bool>(value) ? 1 : 0;
    return (int)std::get<double>(value);
}
const Type VariableValue::getType() const{
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