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

RangeObject::RangeObject(VariableValue min, VariableValue max){
    this->min = std::make_unique<ConstantNode>(ConstantNode(min));
    this->max = std::make_unique<ConstantNode>(ConstantNode(max));
    this->distribution = nullptr;
}

RangeObject::RangeObject(VariableValue val){
    this->min = std::make_unique<ConstantNode>(ConstantNode(val));
    this->max = std::make_unique<ConstantNode>(ConstantNode(val));
    this->distribution = nullptr;
}

RangeObject::RangeObject(std::string str, bool soft){
    this->min = std::make_unique<VariableNode>(VariableNode(str, soft));
    this->max = std::make_unique<VariableNode>(VariableNode(str, soft));
    this->distribution = nullptr;
}

RangeObject::RangeObject(int min, int max){
    this->min = std::make_unique<ConstantNode>(ConstantNode(min));
    this->max = std::make_unique<ConstantNode>(ConstantNode(max));
    this->distribution = nullptr;
}