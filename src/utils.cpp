#include "utils.hpp"
#include "variable.hpp"
#include <memory>
#include "expressiontree.hpp"

std::string formatDouble(double val, bool includeSign){
    if (val == 0) return "0";
    std::string n = std::to_string(val);
    n.erase(n.find_last_not_of('0')+1, n.size()-1);
    if (n[n.size()-1] == '.') n.erase(n.size()-1);
    if (val >= 0 && includeSign) return "+" + n;
    return n;
}

std::unordered_map<std::string, Variable> Defs::vars;

void Defs::addVariable(std::string name, Variable var){
    std::cout << "(" <<  var.getName() << ") <- name of the var\n";
    vars.insert_or_assign(std::move(name), std::move(var));
}

Variable* Defs::getVariable(std::string name){
    if (vars.find(name) == vars.end()) return nullptr;
    return &(vars.at(name));
}