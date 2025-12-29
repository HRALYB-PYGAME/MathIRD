#include "expressiontree.hpp"
#include <stack>

VariableValue ConstantNode::evaluate(GameState& gameState){
    return this->val;
}

VariableValue VariableNode::evaluate(GameState& gameState){
    return gameState.getVar(this->var)->getValue();
}

VariableValue OperandNode::evaluate(GameState& gameState){
    VariableValue left(0);
    if (this->left == nullptr) left = 0;
    else left = this->left->evaluate(gameState);
    VariableValue right = this->right->evaluate(gameState);

    switch(this->oper){
    case Operand::Add:
        return left.add(right);
        break;
    case Operand::Subtract:
        return left.subtract(right);
        break;
    case Operand::Multiply:
        return left.multiply(right);
        break;
    case Operand::Divide:
        return left.divide(right);
        break;
    case Operand::Neg:
        return right.multiply(VariableValue(-1));
    }
    return VariableValue(0);
}

void createSubtree(std::stack<std::unique_ptr<Node>>& nodeStack, Operand oper){
    std::unique_ptr<Node> rightChild = std::move(nodeStack.top()); nodeStack.pop();
    std::unique_ptr<Node> leftChild = nullptr;
    if (!isUnary(oper)){
        leftChild = std::move(nodeStack.top()); nodeStack.pop();
    }
    auto parent = std::make_unique<OperandNode>(oper, std::move(leftChild), std::move(rightChild));
    nodeStack.push(std::move(parent));
}

void printNodeStack(std::stack<std::unique_ptr<Node>>& nodeStack) {
    std::cout << nodeStack.size() << std::endl;
}

std::unique_ptr<Node> construct(std::vector<Token> tokens){
    std::stack<std::unique_ptr<Node>> nodeStack;
    std::stack<Operand> operandStack;
    for(int i=0; i<tokens.size(); i++){
        Token token = tokens[i];
        token.print();
        switch(token.getType()){
        case TokenType::Constant:
            nodeStack.push(std::make_unique<ConstantNode>(VariableValue(token.getValueAsDouble())));
            break;
        case TokenType::Variable:
            nodeStack.push(std::make_unique<VariableNode>(token.getValueAsString()));
            break;
        case TokenType::Operand:
            Operand oper = token.getValueAsOperand();
            if (isFunction(oper)){
                operandStack.push(oper);
            }
            else if (oper == Operand::LeftPar){
                operandStack.push(oper);
            }
            else if (oper == Operand::RightPar){
                while(operandStack.top() != Operand::LeftPar){
                    Operand topOper = operandStack.top(); operandStack.pop();
                    createSubtree(nodeStack, topOper);
                }
                operandStack.pop();
                if (!operandStack.empty() && isFunction(operandStack.top())){
                    Operand fun = operandStack.top();
                    createSubtree(nodeStack, fun);
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
        }
        printNodeStack(nodeStack);
    }

    while(!operandStack.empty()){
        Operand topOper = operandStack.top(); operandStack.pop();
        Token(topOper).print();
        createSubtree(nodeStack, topOper);
        printNodeStack(nodeStack);
    }

    return std::move(nodeStack.top());
}