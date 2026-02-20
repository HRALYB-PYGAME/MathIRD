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
    if (btns.find(btn.getName()) != btns.end())
        Log::warn("Multiple definitions of button \"" + btn.getName() + "\"");
    btns.insert_or_assign(btn.getName(), std::move(btn));
}

// returns a pointer to a button with given name or a nullptr if not found
Button* Defs::getButton(std::string name){
    auto it = btns.find(name);
    if (it == btns.end()) return nullptr;
    return &(it->second);
}

void Defs::addProcess(Process proc){
    if (procs.find(proc.getName()) != procs.end())
        Log::warn("Multiple definitions of process \"" + proc.getName() + "\"");
    procs.insert_or_assign(proc.getName(), std::move(proc));
}

Process* Defs::getProcess(std::string name){
    auto it = procs.find(name);
    if (it == procs.end()) return nullptr;
    return &(it->second);
}


void Defs::loadButtons(std::string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        Log::setContext(entry.path().string());
        LOG("button loading, new entry");
        if (!entry.is_regular_file()) continue;

        LOG("button loading, regular file");

        std::ifstream file(entry.path());

        if (!file.is_open()){
            Log::warn("Failed to open");
            continue;
        }

        json j = json::parse(file);

        LOG("button loading, parsed");

        if (!j.contains("name")){
            Log::error("Missing required field \"name\"");
        }

        if (!j.contains("position")){
            Log::error("Missing required field \"position\"");
        }

        LOG("button loading, name & positions");

        std::string name = j["name"];

        Button button(name);

        LOG("button loading, empty button object");

        if (j.contains("displayStates")){
            auto& displayStates = j["displayStates"];
            for(const auto& state : displayStates){
                if (!state.contains("text"))
                    Log::error("Missing required field \"displayStates.text\"");
                if (!state.contains("condition"))
                    Log::warn("Missing field \"displayStates.condition\", defaulting to \"1\"");
                Expression condition = construct(tokenize(state.value("condition", "1")));
                condition.normalize();
                button.setDisplay(std::move(condition.expr), state["text"]);
            }
        }
        else{
            Log::warn("Missing field \"displayStates\"");
        }

        LOG("button loading, displayStates");

        auto& jp = j["position"];
        button.setPosition(jp.value("row", 0), jp.value("col", 0));

        LOG("button loading, positios");

        if (j.contains("terms")){
            auto& terms = j["terms"];
            for(const auto& t : terms){
                if (!t.contains("name")) 
                    Log::error("Missing required field \"terms.name\"");
                if (!t.contains("expressions"))
                    Log::error("Missing required field \"terms.expressions\"");
                Term term(t["name"]);
                if (!t.contains("condition"))
                    Log::warn("Missing field \"terms.condition\", defaulting to \"1\"");
                Expression condition = construct(tokenize(t.value("condition", "1")));
                condition.normalize();
                term.setCondition(std::move(condition.expr));
                for(const auto& expr : t["expressions"]){
                    Expression expression = construct(tokenize(expr));
                    term.addExpression(std::move(expression)); // problem
                }
                if (t["expressions"].size() == 0)
                    Log::warn("Unspecified field \"terms.name\"");
                LOG("before adding new term\n");
                button.addTerm(std::make_unique<Term>(std::move(term)));
            }
        }
        else
            Log::warn("Missing field \"terms\"");

        LOG("button loading, terms");

        Defs::addButton(std::move(button));

        Log::info("Succesfully loaded");
    }
}

Polarity stringToPolarity(std::string polarity){
    if (polarity == "normal") return Polarity::Normal;
    if (polarity == "inverted") return Polarity::Inverted;
    if (polarity == "absolute") return Polarity::Absolute;
    if (polarity == "neutral") return Polarity::Neutral;
    Log::warn("Invalid value for \"polarity\": " + polarity + ", defaulting to \"neutral\", expected \"absolute\", \"inverted\", \"neutral\" or \"normal\"");
    return Polarity::Neutral;
}

void Defs::loadVariables(std::string path, std::unordered_map<std::string, std::string>& linkerMap){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        Log::setContext(entry.path().string());
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (!file.is_open()){
            Log::warn("Failed to open");
            continue;
        }

        json j = json::parse(file);

        if (!j.contains("name")){
            Log::error("Missing required field \"name\"");
        }

        std::string name = j["name"];

        VariableType type = VariableType::Double;

        if (j.contains("type")){
            if (j["type"] == "B")
                type = VariableType::Boolean;
            else if (j["type"] == "D")
                type = VariableType::Double;
            else if (j["type"] == "I")
                type = VariableType::Int;
            else if (j["type"] == "E"){
                type = VariableType::Enum;
                if (!j.contains("states"))
                    Log::error("Missing required field \"type.states\" (required for \"Enum\" type variables)");
                if (j["states"].empty())
                    Log::error("Unspecified field \"type.states\"  (required for \"Enum\" type variables)");
            }
            else if (j["type"] == "P")
                type = VariableType::Percentage;
            else
                Log::error("Invalid value for field \"type\": " + j["type"].dump() + ", expected \"B\", \"D\", \"E\", \"I\" or \"P\"");
        }
        else{
            Log::warn("Missing field \"type\", defaulting to \"Double\"");
        }

        ScoreParams scoreParams = {100, 0, Polarity::Neutral};;
        if (j.contains("scoreParams")){
            auto& js = j["scoreParams"];
            if (!js.contains("knee"))
                Log::warn("Missing field \"knee\", defaulting to 100");
            scoreParams.knee = js.value("knee", 100);
            if (!js.contains("offset"))
                Log::warn("Missing field \"offset\", defaulting to 0");
            scoreParams.offset = js.value("offset", 0);
            if (!js.contains("polarity"))
                Log::warn("Missing field \"polarity\", defaulting to \"neutral\"");
            scoreParams.polarity = stringToPolarity(js.value("polarity", "neutral"));
        }
        else
            Log::warn("Missing field \"scoreParams\", defaulting to \"knee\"=100, \"offset\"=0, \"polarity\"=\"neutral\"");

        if (j.contains("homeButton")){
            linkerMap[name] = j["homeButton"];
        }
        else
            Log::error("Missing required field \"homeButton\"");

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
        else
            Log::warn("Missing field \"granularity\", defaulting to 1");

        if (var.getType() == VariableType::Enum){
            for(auto state : j["states"]){
                if (var.doesStateExist(state))
                    Log::warn("State \"" + state.dump() + "\" already exists");
                var.addState(state);
            }
        }

        Defs::addVariable(std::move(var));

        Log::info("Succesfully loaded");
    }
}

void Defs::loadProcesses(std::string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        Log::setContext(entry.path().string());
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (!file.is_open()){
            Log::warn("Failed to open");
            continue;
        }

        json j = json::parse(file);

        if (!j.contains("name")){
            Log::error("Missing required field \"name\"");
        }

        std::string name = j["name"];

        Process process(name);

        if (j.contains("startCondition")){
            auto& cond = j["startCondition"];
            Expression condition = construct(tokenize(cond));
            condition.normalize();
            process.setStartCondition(std::move(condition.expr));
        }
        else{
            Log::error("Missing required field \"startCondition\"");
        }

        if (j.contains("endCondition")){
            auto& cond = j["endCondition"];
            Expression condition = construct(tokenize(cond));
            condition.normalize();
            process.setEndCondition(std::move(condition.expr));
        }
        else{
            std::string cond = "!(" + j["startCondition"].dump() + ")";
            Expression condition = construct(tokenize(cond));
            condition.normalize();
            process.setEndCondition(std::move(condition.expr));
            Log::warn("Missing field \"endCondition\", defaulting to !(startCondition)");
        }

        if (j.contains("interval")){
            auto& inter = j["interval"];
            Expression interval = construct(tokenize(inter));
            process.setInterval(std::move(interval.expr));
        }
        else
            Log::error("Missing required field \"interval\"");

        if (j.contains("terms")){
            auto& terms = j["terms"];
            for(const auto& t : terms){
                if (!t.contains("name")) 
                    Log::error("Missing required field \"terms.name\"");
                if (!t.contains("expressions"))
                    Log::error("Missing required field \"terms.expressions\"");
                Term term(t["name"]);
                if (!t.contains("condition"))
                    Log::warn("Missing field \"terms.condition\", defaulting to \"1\"");
                Expression condition = construct(tokenize(t.value("condition", "1")));
                condition.normalize();
                term.setCondition(std::move(condition.expr));
                for(const auto& expr : t["expressions"]){
                    Expression expression = construct(tokenize(expr));
                    term.addExpression(std::move(expression)); // problem
                }
                if (t["expressions"].size() == 0)
                    Log::warn("Unspecified field \"terms.name\"");
                process.addTerm(std::make_unique<Term>(std::move(term)));
            }
        }
        else
            Log::warn("Missing field \"terms\"");

        Defs::addProcess(std::move(process));

        Log::info("Succesfully loaded");
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

void Defs::bind(){
    for(auto& [varName, var] : vars){
        var.bind();
    }
    for(auto& [btnName, btn] : btns){
        btn.bind();
    }
    for(auto& [procName, proc] : procs){
        proc.bind();
    }

    LOG("binded");

    for(auto& [btnName, btn] : btns){
        btn.updateTermsSets();
    }

    LOG("updated button terms sets");
    
    for(auto& [procName, proc] : procs){
        proc.updateTermsSets();
    }

    LOG("sets updated");
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

void Log::error(const std::string& message) {
    std::cerr 
    << "\033[31m" << std::left << std::setw(typeWidth)
    << "[ERROR] "
    << std::left << std::setw(pathWidth) <<
    currentFile << message << "\033[0m" << std::endl;
    std::exit(EXIT_FAILURE);
}

void Log::warn(const std::string& message) {
    std::cout << "\033[33m" << std::left << std::setw(typeWidth)
    << "[ERROR] "
    << std::left << std::setw(pathWidth) <<
    currentFile << message << "\033[0m" << std::endl;
}

void Log::info(const std::string& message) {
    std::cout << "\033[32m" << std::left << std::setw(typeWidth)
    << "[ERROR] "
    << std::left << std::setw(pathWidth) <<
    currentFile << message << "\033[0m" << std::endl;
}