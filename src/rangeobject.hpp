#ifndef rangeobject_hpp
#define rangeobject_hpp
#include <memory>
#include <string>
#include "variable_value.hpp"

class Node;
enum class Operand : int;

class RangeObject{
public:
    std::unique_ptr<Node> min, max;
    std::unique_ptr<Node> distribution;

    void combine(RangeObject& ro, Operand oper);

    RangeObject(VariableValue min, VariableValue max);
    RangeObject(VariableValue val);
    RangeObject(std::string str, bool soft);
    RangeObject(int min, int max);
};

#endif