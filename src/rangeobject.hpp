#ifndef rangeobject_hpp
#define rangeobject_hpp
#include <memory>
#include <string>
#include "variable_value.hpp"

class Node;
enum class Operand : int;

struct RangeObject{
    std::unique_ptr<Node> min, max;
    std::unique_ptr<Node> distribution;

    void combine(RangeObject& ro, Operand oper);

    RangeObject(double min, double max);
    RangeObject(double val);
    RangeObject(std::string str, VariableFlags flags);
};

#endif