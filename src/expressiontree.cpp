#include "expressiontree.hpp"
#include <stack>

using namespace std::string_literals;

void createSubtree(std::stack<std::unique_ptr<Node>>& nodeStack, Operand oper){
    if (nodeStack.empty()) {
        LOG("CRITICAL ERROR: Stack empty before popping right child!");
        return; 
    }
    
    LOG("expressiontree.cpp\tcreating subtree with nodestack of size: " << nodeStack.size());
    std::unique_ptr<Node> rightChild = std::move(nodeStack.top()); nodeStack.pop();
    std::unique_ptr<Node> leftChild = nullptr;
    if (!isUnary(oper)){
        if (nodeStack.empty()) {
            LOG("CRITICAL ERROR: Stack empty before popping left child for binary op!");
            // Safety: Put the right child back or handle the error
            nodeStack.push(std::move(rightChild));
            return;
        }
        leftChild = std::move(nodeStack.top()); nodeStack.pop();
    }
    auto parent = std::make_unique<OperandNode>(oper, std::move(leftChild), std::move(rightChild));
    nodeStack.push(std::move(parent));
    LOG("expressiontree.cpp\tcreating subtree with nodestack of size: " << nodeStack.size() << " succesfull");
}

void printNodeStack(std::stack<std::unique_ptr<Node>>& nodeStack) {
    std::cout << nodeStack.size() << std::endl;
}

Expression construct(std::vector<Token> tokens){
    std::set<std::string> variableLocks;
    std::stack<std::unique_ptr<Node>> nodeStack;
    std::stack<Operand> operandStack;

    bool neg = false;
    bool inFunction = false;
    FunctionNode functionNode("", {});
    for(size_t i=0; i<tokens.size(); i++){
        Token token = tokens[i];
        //std::cout << "nodestack size " << nodeStack.size() << " operandStack size " << operandStack.size() << std::endl;
        //token.print();
        switch(token.getType()){
        case TokenType::Constant:
            nodeStack.push(std::make_unique<ConstantNode>(token.getValueAsDouble() * (neg ? -1 : 1)));
            break;
        case TokenType::Variable:
            nodeStack.push(std::make_unique<VariableNode>(token.getValueAsString(), token.getFlags()));
            break;
        case TokenType::VariableLock:
            variableLocks.insert(token.getValueAsString());
            break;
        case TokenType::Function:
            functionNode.name = token.getValueAsString();
            inFunction = true;
            break;
        case TokenType::Argument:
            functionNode.args.push_back(token.getValueAsString());
            break;
        case TokenType::Operand:
            Operand oper = token.getValueAsOperand();
            if (oper == Operand::Neg){
                neg = true;
            }
            else if (isFunction(oper)){
                operandStack.push(oper);
            }
            else if (oper == Operand::LeftPar){
                if (!inFunction)
                    operandStack.push(oper);
            }
            else if (oper == Operand::RightPar){
                if (inFunction){
                    nodeStack.push(std::make_unique<FunctionNode>(functionNode));
                    functionNode.args.clear();
                    inFunction = false;
                    break;
                }
                while(operandStack.top() != Operand::LeftPar){
                    Operand topOper = operandStack.top(); operandStack.pop();
                    createSubtree(nodeStack, topOper);
                }
                operandStack.pop();
                if (!operandStack.empty() && isFunction(operandStack.top())){
                    Operand fun = operandStack.top();
                    createSubtree(nodeStack, fun);
                    operandStack.pop();
                }
            }
            else{
                while(!operandStack.empty() && (getPrecedence(operandStack.top()) > getPrecedence(oper) ||
                    (getPrecedence(operandStack.top()) == getPrecedence(oper) && isLeftAssociative(operandStack.top())))){
                    Operand topOper = operandStack.top(); operandStack.pop();
                    createSubtree(nodeStack, topOper);
                }
                operandStack.push(oper);
            }
            break;
        }
    }

    while(!operandStack.empty()){
        Operand topOper = operandStack.top(); operandStack.pop();
        createSubtree(nodeStack, topOper);
    }

    LOG("expressiontree.cpp\texpression business coming next\n");
    LOG("nodestack size: " << nodeStack.size());

    Expression expr(nodeStack.top()->clone(), {});

    LOG("expressiontree.cpp\texpression object created succesfully\n");

    if (nodeStack.top()->getType() == NodeType::Operand){
        for(auto& lock : variableLocks){
            expr.variableLocks.insert(lock);
        }
    }
    
    return expr;
}