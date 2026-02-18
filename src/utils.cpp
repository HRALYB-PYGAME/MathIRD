#include "utils.hpp"
#include "button.hpp"
#include "process.hpp"

#include <memory>
#include <filesystem>
#include "../libs/json/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::string formatDouble(double val, bool includeSign){
    if (val == 0) return "0";
    std::string n = std::to_string(val);
    n.erase(n.find_last_not_of('0')+1, n.size()-1);
    if (n[n.size()-1] == '.') n.erase(n.size()-1);
    if (val >= 0 && includeSign) return "+" + n;
    return n;
}

std::unordered_map<std::string, Variable> Defs::vars;
std::unordered_map<std::string, Button>   Defs::btns;
std::unordered_map<std::string, Process>  Defs::procs;

void Defs::addVariable(Variable var){
    vars.insert_or_assign(var.getName(), std::move(var));
}

/// @brief Returns a pointer to a Variable with a given name.
/// @param name name of a Variable to be found
/// @return Pointer to a Variable, nullptr if not found. 
Variable* Defs::getVariable(std::string name){
    if (vars.find(name) == vars.end()) return nullptr;
    return &(vars.at(name));
}

void Defs::addButton(Button btn){
    btns.insert_or_assign(btn.getName(), std::move(btn));
}

// returns a pointer to a button with given name or a nullptr if not found
Button* Defs::getButton(std::string name){
    auto it = btns.find(name);
    if (it == btns.end()) return nullptr;
    return &(it->second);
}

void Defs::addProcess(Process proc){
    LOG("Defs::addProcess(Process=\"" << proc.getName() << "\")");
    procs.insert_or_assign(proc.getName(), std::move(proc));
}

Process* Defs::getProcess(std::string name){
    auto it = procs.find(name);
    if (it == procs.end()) return nullptr;
    return &(it->second);
}


void Defs::loadButtons(std::string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (!file.is_open()) continue;

        json j = json::parse(file);

        if (!j.contains("name")){
            std::cerr << entry.path().filename() << " missing 'name'. Skipping." << std::endl;
            continue;
        }

        if (!j.contains("position")){
            std::cerr << entry.path().filename() << " missing 'position'. Skipping." << std::endl;
            continue;
        }

        std::string name = j["name"];

        Button button(name);

        if (j.contains("displayStates")){
            auto& displayStates = j["displayStates"];
            for(const auto& state : displayStates){
                if (!state.contains("text")) continue;
                Expression condition = construct(tokenize(state.value("condition", "1")));
                condition.normalize();
                button.setDisplay(std::move(condition.expr), state["text"]);
                if (!state.contains("condition")) break;
            }
        }

        auto& jp = j["position"];
        button.setPosition(jp.value("row", 0), jp.value("col", 0));

        if (j.contains("terms")){
            auto& terms = j["terms"];
            for(const auto& t : terms){
                if (!t.contains("name")) continue;
                if (!t.contains("expressions")) continue;
                Term term(t["name"]);
                Expression condition = construct(tokenize(t.value("condition", "1")));
                condition.normalize();
                term.setCondition(std::move(condition.expr));
                for(const auto& expr : t["expressions"]){
                    Expression expression = construct(tokenize(expr));
                    term.addExpression(std::move(expression));
                }
                button.addTerm(std::make_unique<Term>(std::move(term)));
            }
        }

        Defs::addButton(std::move(button));
    }
}

Polarity stringToPolarity(std::string polarity){
    if (polarity == "normal") return Polarity::Normal;
    if (polarity == "inverted") return Polarity::Inverted;
    if (polarity == "absolute") return Polarity::Absolute;
    return Polarity::Neutral;
}

void Defs::loadVariables(std::string path, std::unordered_map<std::string, std::string>& linkerMap){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (!file.is_open()) continue;

        json j = json::parse(file);

        if (!j.contains("name")){
            LOG("utils.cpp\tDefs::loadVariables " << entry.path().filename() << " missing 'name'. Skipping.");
            continue;
        }

        std::string name = j["name"];

        VariableType type = VariableType::Double;

        if (j.contains("type")){
            if (j["type"] == "B")
                type = VariableType::Boolean;
            if (j["type"] == "I")
                type = VariableType::Int;
            if (j["type"] == "E"){
                type = VariableType::Enum;
                if (!j.contains("states")) continue;
                if (j["states"].empty()) continue;
            }
            if (j["type"] == "P")
                type = VariableType::Percentage;
        }

        ScoreParams scoreParams = {100, 0, Polarity::Neutral};;
        if (j.contains("scoreParams")){
            auto& js = j["scoreParams"];
            scoreParams.knee = js.value("knee", 100);
            scoreParams.offset = js.value("offset", 0);
            scoreParams.polarity = stringToPolarity(js.value("polarity", "neutral"));
        }

        if (j.contains("homeButton")){
            linkerMap[name] = j["homeButton"];
        }

        Expression condition = construct(tokenize(j.value("unlockCondition", "1")));
        condition.normalize();
        Variable var(
            name, 
            scoreParams, 
            std::move(condition.expr), 
            j.value("defaultValue", 0.0),
            type
        );

        if (j.contains("granularity")){
            var.setGranularity(j["granularity"]);
        }

        if (var.getType() == VariableType::Enum){
            for(auto state : j["states"]){
                var.addState(state);
            }
        }

        Defs::addVariable(std::move(var));
    }
}

void Defs::loadProcesses(std::string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (!file.is_open()) continue;

        json j = json::parse(file);

        if (!j.contains("name")){
            std::cerr << entry.path().filename() << " missing 'name'. Skipping." << std::endl;
            continue;
        }

        std::string name = j["name"];

        Process process(name);

        if (j.contains("startCondition")){
            auto& cond = j["startCondition"];
            Expression condition = construct(tokenize(cond));
            condition.normalize();
            process.setStartCondition(std::move(condition.expr));
        }

        if (j.contains("endCondition")){
            auto& cond = j["endCondition"];
            Expression condition = construct(tokenize(cond));
            condition.normalize();
            process.setEndCondition(std::move(condition.expr));
        }

        if (j.contains("interval")){
            auto& inter = j["interval"];
            Expression interval = construct(tokenize(inter));
            process.setInterval(std::move(interval.expr));
        }

        if (j.contains("terms")){
            auto& terms = j["terms"];
            for(const auto& t : terms){
                if (!t.contains("name")) continue;
                if (!t.contains("expressions")) continue;
                Term term(t["name"]);
                Expression condition = construct(tokenize(t.value("condition", "1")));
                condition.normalize();
                term.setCondition(std::move(condition.expr));
                for(const auto& expr : t["expressions"]){
                    Expression expression = construct(tokenize(expr));
                    term.addExpression(std::move(expression));
                }
                process.addTerm(std::make_unique<Term>(std::move(term)));
            }
        }

        Defs::addProcess(std::move(process));
    }
}

void Defs::linkVariableHomeButtons(std::unordered_map<std::string, std::string>& linkerMap){
    LOG("utils.cpp\tlinkVariableHomeButtons() FUNCTION BEG.");
    for(auto [varName, btnName] : linkerMap){
        Button* btn = getButton(btnName);
        if (btn != nullptr){
            LOG("utils.cpp\tlinkVariableHomeButtons() " << varName << "linked to (" << btnName << ").");
            Variable* var = getVariable(varName);
            if (var != nullptr) var->setHomeButton(*btn);
        }
        else LOG("utils.cpp\tlinkVariableHomeButtons() Button specified in " << varName << "s config doesnt exist (" << btnName << ").");
    }
}

Insightable* Defs::getTarget(std::string targetPath){
    size_t colonPos = targetPath.find(':');
    if (colonPos == std::string::npos) return nullptr;
    std::cout << "colon found\n";

    char prefix = targetPath[0];
    std::cout << "prefix: " << prefix << "\n";
    targetPath = targetPath.substr(colonPos + 1);
    std::cout << "new targetpath: " << targetPath << "\n";

    size_t dotPos = targetPath.find('.');
    std::string parentName = targetPath.substr(0, dotPos);
    std::cout << "parentName: " << parentName << "\n";
    std::string childName = (dotPos != std::string::npos) ? targetPath.substr(dotPos + 1) : "";
    std::cout << "childName: " << childName << "\n";

    Insightable* parent = nullptr;
    if (prefix == 'P') parent = getProcess (parentName);
    if (prefix == 'B') parent = getButton  (parentName);
    if (prefix == 'V') parent = getVariable(parentName);
    if (!parent) return nullptr;

    std::cout << "parent found\n";

    if (!childName.empty()){
        return parent->getTerm(childName);
    }
    return parent;
}

double getDistance(ButtonPosition start, ButtonPosition end){
    return sqrt(pow(start.row - end.row, 2) + pow(start.col - end.col, 2));
}