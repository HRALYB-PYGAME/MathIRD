#include <iostream>
#include "variable.hpp"

int main(){
    std::cout << "Henlo" << std::endl;
    Variable var("x", VariableValue(100), {100, 0, Polarity::Normal});
    var.add(-50);
    std::cout << var.getScore() << std::endl;
    return 0;
}