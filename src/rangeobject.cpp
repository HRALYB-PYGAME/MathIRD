#include "rangeobject.hpp"
#include "expressiontree.hpp"
#include "tokenizer.hpp"

void RangeObject::combine(RangeObject& ro, Operand oper){
    this->min = std::make_unique<OperandNode>(OperandNode(oper, std::move(this->min), std::move(ro.min)));
    this->max = std::make_unique<OperandNode>(OperandNode(oper, std::move(this->max), std::move(ro.max)));
    if (ro.distribution != nullptr){
        this->distribution = std::make_unique<OperandNode>(OperandNode(oper, 
            std::move(this->distribution), std::move(ro.distribution)));
    }
}

RangeObject::RangeObject(double min, double max){
    this->min = std::make_unique<ConstantNode>(ConstantNode(min));
    this->max = std::make_unique<ConstantNode>(ConstantNode(max));
    this->distribution = nullptr;
}

RangeObject::RangeObject(double val){
    this->min = std::make_unique<ConstantNode>(ConstantNode(val));
    this->max = std::make_unique<ConstantNode>(ConstantNode(val));
    this->distribution = nullptr;
}

RangeObject::RangeObject(std::string str, VariableFlags flags){
    if (str == "_R" || str == "_NR"){
        this->min = std::make_unique<ConstantNode>(ConstantNode(0));
        this->max = std::make_unique<ConstantNode>(ConstantNode(1));
        this->distribution = std::make_unique<VariableNode>(VariableNode(str, flags));
    }
    else{
        this->min = std::make_unique<VariableNode>(VariableNode(str, flags));
        this->max = std::make_unique<VariableNode>(VariableNode(str, flags));
        this->distribution = nullptr;
    }
}