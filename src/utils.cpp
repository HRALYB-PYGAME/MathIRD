#include "utils.hpp"
#include "variable.hpp"
#include <memory>
#include "expressiontree.hpp"
#include "button.hpp"
#include <filesystem>
#include "../libs/json/json.hpp"
#include <fstream>
#include <iostream>

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

void Defs::addVariable(Variable var){
    vars.insert_or_assign(var.getName(), std::move(var));
}

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

void Defs::loadVariables(std::string path, std::unordered_map<std::string, std::string>& linkerMap){
    using json = nlohmann::json;

    for (const auto& entry : std::filesystem::directory_iterator(path)){
        if (!entry.is_regular_file()) continue;

        std::ifstream file(entry.path());

        if (file.is_open()){
            json j = json::parse(file);
            std::string name;
            ScoreParams scoreParams = {100, 0, Polarity::Neutral};
            double defaultValue = 0;
            std::string homeButton = "";
            std::string condition = "1";

            if (j.find("name") == j.end()){
                std::cerr << entry.path().filename() << " doesnt contain name attribute. Skipping." << std::endl;
                continue;
            }
            name = j["name"];

            if (j.find("scoreParams") == j.end()){
                std::cerr << entry.path().filename() << " doesnt contain scoreParams attribute. Using default." << std::endl;
            }
            else{
                json jScoreParams = j["scoreParams"];

                if (jScoreParams.find("knee") == jScoreParams.end()){
                    std::cerr << entry.path().filename() << " doesnt contain knee attribute. Using default (100)." << std::endl;
                }
                else scoreParams.knee = jScoreParams["knee"];

                if (jScoreParams.find("offset") == jScoreParams.end()){
                    std::cerr << entry.path().filename() << " doesnt contain offset attribute. Using default (0)." << std::endl;
                }
                else scoreParams.offset = jScoreParams["offset"];

                if (jScoreParams.find("polarity") == jScoreParams.end()){
                    std::cerr << entry.path().filename() << " doesnt contain knee attribute. Using default (neutral)." << std::endl;
                }
                else{
                    std::string pol = jScoreParams["polarity"];
                    if (pol == "normal"){
                        scoreParams.polarity = Polarity::Normal;
                    }
                    else if (pol == "neutral"){
                        scoreParams.polarity = Polarity::Neutral;
                    }
                    else if (pol == "inverted"){
                        scoreParams.polarity = Polarity::Inverted;
                    }
                    else if (pol == "absolute"){
                        scoreParams.polarity = Polarity::Absolute;
                    }
                    else{
                        std::cerr << entry.path().filename() << " contains an invalid polarity name (" << pol << "). Using default (normal)." << std::endl;
                        scoreParams.polarity = Polarity::Normal;
                    }
                }
            }

            if (j.find("defaultValue") == j.end())
                std::cerr << entry.path().filename() << " doesnt contain defaultValue attribute. Using 0 as default." << std::endl;
            else
                defaultValue = j["defaultValue"];

            if (j.find("homeButton") != j.end())
                linkerMap.insert_or_assign(name, j["homeButton"]);

            if (j.find("unlockCondition") == j.end()){
                std::cerr << entry.path().filename() << " doesnt contain unlockCondition attribute. Using \"1\" as default." << std::endl;
            }
            else{
                condition = j["unlockCondition"];
            }

            Variable var(name, scoreParams, construct(tokenize(condition)), VariableValue(defaultValue));

            Defs::addVariable(std::move(var));
        }
    }
}

void Defs::linkVariableHomeButtons(std::unordered_map<std::string, std::string>& linkerMap){
    for(auto [varName, btnName] : linkerMap){
        Button* btn = getButton(btnName);
        if (btn != nullptr){
            Variable* var = getVariable(varName);
            if (var != nullptr) var->setHomeButton(*btn);
        }
        else std::cout << "Button specified in " << varName << "'s config doesnt exist (" << btnName << ").\n";
    }
}