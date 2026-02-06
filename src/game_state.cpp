#include "game_state.hpp"
#include <iostream>

GameState::GameState() : currentInsight() {
}

double GameState::getTotalScore(){
    double score = 0;
    for (auto& [name, varEntry] : variables){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr)
            score += var->getScoreParams().getScore(varEntry.value);
    }
    return score;
}

void GameState::applyDelta(std::string var, double delta){
    LOG("game_state.cpp\tapplyDelta(var=" << var << ", delta=" << delta << ") FUNCTION BEG");
    addVarValue(var, delta);
    LOG("game_state.cpp\tapplyDelta(var=" << var << ", delta=" << delta << ") ADDED");
}

void GameState::applyDeltas(VariableChanges changes){
    LOG("game_state.cpp\tapplyDeltas(changes) FUNCTION BEG");
    for (auto& [var, val] : changes.changes){
        addVarValue(var, val.rand);
        LOG("game_state.cpp\tapplyDeltas(changes) ADDED (" << var << " value=" << getVarValueAsDouble(var) << ")");
    }
    this->updateVariables();
    LOG("game_state.cpp\tapplyDeltas(changes) VARIABLES UPDATED");
}

void GameState::applyNewValue(std::string var, double newValue){
    LOG("game_state.cpp\tapplyChange(var=" << var << ", newValue=" << newValue << ") FUNCTION BEG");
    setVarValue(var, newValue);
    LOG("game_state.cpp\tapplyChange(var=" << var << ", newValue=" << newValue << ") ADDED");
}

void GameState::setVarValue(std::string name, double value){
    LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << ") FUNCTION BEG");
    auto it = variables.find(name);
    if (it != variables.end()){
        it->second.value = value;
        it->second.incrementVersion();
        LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << " version=" << variables.at(name).version << ") VALUE SET");
    }
    else
        LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << ") " << name << " NOT FOUND");
}  

void GameState::addVarValue(std::string name, double value){
    LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") FUNCTION BEG");
    if (variables.empty()){
        LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") VARIABLES MAP EMPTY");
        return;
    }

    auto it = variables.find(name);
    if (it != variables.end()){
        it->second.value += value;
        variables.at(name).incrementVersion();
        LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") VALUE ADDED");
    }
    else
        LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") " << name << " NOT FOUND");
}

void GameState::updateVariables(){
    for (auto& [name, entry] : variables){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr){
            if (!isVariableUnlocked(name) && var->getUnlockCondition().evaluate(*this)){
                LOG("game_state.cpp\tupdateVariables() " << name << " UNLOCKED");
                entry.unlock();
            }
        }
    }
}

bool GameState::isVariableUnlocked(std::string name){
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).unlocked;
}

void GameState::blockVariable(std::string name){
    if (variables.find(name) != variables.end())
        variables.at(name).blockCounter++;
}

void GameState::unblockVariable(std::string name){
    if (variables.find(name) != variables.end())
        variables.at(name).blockCounter--;
}

bool GameState::isVariableBlocked(std::string name){
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).blockCounter!=0;
}

int GameState::getVariableBlockCounter(std::string name){
    auto it = variables.find(name);
    if (it != variables.end()) return it->second.blockCounter;
    return 0;
}

void GameState::addVariable(Variable* variable){
    if (variables.count(variable->getName())) return;
    VariableEntry entry(variable->getDefaultValue());
    std::string name = variable->getName();
    variables.insert({name, entry});
}

void GameState::addVariables(){
    LOG("why is ADDVARIABLES CALLED?? I SHOULD ONLY CALL IT AT THE START AND THEN ONLY UPDATE");
    for (auto& [name, var] : Defs::vars){
        addVariable(&var);
    }
    updateVariables();
}

VariableEntry::VariableEntry(double value){
    this->value = value;
    this->realValue = value;
    lock();
}

ButtonEntry::ButtonEntry(){
    lock();
}

void GameState::addButton(Button* button){
    ButtonEntry entry;
    std::string name = button->getName();
    buttons.insert({name, entry});
}

void GameState::addButtons(){
    for(auto& [name, btn] : Defs::btns){
        addButton(&btn);
    }
    updateButtons();
}

bool GameState::isButtonUnlocked(std::string name){
    if (buttons.find(name) == buttons.end()) return false;
    return buttons.at(name).unlocked;
}

void GameState::updateButtons(){
    for (auto& [name, entry] : buttons){
        Button* btn = Defs::getButton(name);

        if (btn == nullptr) continue;
        
        if (!isButtonUnlocked(name) && btn->isUnlocked(*this))
            entry.unlock();
    }
}

void GameState::printUnlocked(){
    for (auto& [name, entry] : variables){
        if (entry.unlocked)
            std::cout << "unlocked: " << name << std::endl;
    }
}

double GameState::getCurrentrandom(){
    return currentSeed;
}

void GameState::step(){
    uint64_t multiplier = 6364136223846793005ULL;
    uint64_t increment = 1442695040888963407ULL;
    currentSeed = currentSeed*multiplier + increment;
}

double GameState::insertNewConditionResult(bool result){
    auto& vec = probabilities[currentTerm];
    if (vec.size() >= currentIndex){
        vec.resize(currentIndex+1);
    }
    return vec[currentIndex].update(result);
}

int GameState::getVarVersion(std::string name){
    if (variables.find(name) != variables.end()){
        return variables.at(name).version;
    }
    return 0;
}

double GameState::getVarValue(std::string name){
    if (name == "_R"){
        if (forcedRandom != -1) return forcedRandom;
        return currentSeed/RANDOM_MAX;
    }
    if (name == "_NR"){
        step();
        if (forcedRandom != -1) return forcedRandom;
        return currentSeed/RANDOM_MAX;
    }
    if (this->variables.find(name) != this->variables.end()){
        return variables.at(name).value;
    }

    //auto var = Defs::getVariable(name);
    //if (var == nullptr)
        return 0;
    //addVariable(var);
    //return getVarValue(name);
}

double GameState::getVarValueAsDouble(std::string name){
    if (name == "_R"){
        if (forcedRandom != -1) return forcedRandom; 
        return currentSeed/RANDOM_MAX;
    }
    if (name == "_NR"){
        step();
        if (forcedRandom != -1) return forcedRandom;
        return currentSeed/RANDOM_MAX;
    }

    auto it = variables.find(name);
    if (it != variables.end()){
        return it->second.value;
    }
    
    // auto var = Defs::getVariable(name);
    // if (var == nullptr)
        return 0.0;
    // addVariable(var);
    // return getVarValueAsDouble(name);
}

void GameState::addPacket(Packet packet) {
    // packets with lowest arrivalTime at the end
    auto it = packets.begin();

    while (it != packets.end()){
        if (it->arrivalTime <= packet.arrivalTime)
            break;
        it++;
    }

    LOG("game_state.cpp\taddPacket() variable=" << packet.variable);
    std::string name = packet.variable;
    for(auto lock : packet.variableLocks){
        blockVariable(lock);

    }

    packets.insert(it, std::move(packet));

    it = packets.begin();
    while (it != packets.end()){
        std::cout << it->arrivalTime.time_since_epoch().count() << "\n";
        it++;
    }

    updateRealValue(name);
}

void GameState::addPackets(std::vector<Packet>& packets) {
    for(auto& packet : packets){
        addPacket(std::move(packet));
    }
    packets.clear();
}

void GameState::updateRealValue(std::string name){
    setRealValue(name, getVarValue(name));
    LOG("game_state.cpp\tupdateRealValue(name=" << name << ")");
    for(int i=(int)packets.size()-1; i>=0; i--){
        if (packets[i].variable != name) continue;
        auto newValue = packets[i].expression->evaluate(*this, true);
        setRealValue(name, newValue);
    }
}

void GameState::addPacketFromAnExpression(const Expression& expression, std::vector<Packet>& packets, ButtonPosition startPos, Clock::time_point time, uint64_t& seed){
    auto outputs = expression.expr->getOutputs(true);
    if (outputs.empty()) return;
    std::string name = *outputs.begin();

    std::unique_ptr<Node> expr = expression.expr->getPacketExpression(*this, true);
    

    LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << "?");
    Variable* var = Defs::getVariable(name);

    Packet p;
    p.variable = name;
    p.variableLocks = expression.variableLocks;
    p.expression = std::move(expr);
    p.startPos = startPos;
    if (var->getHomeButton() == nullptr){
        LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " HAVE AN INVALID HOME BUTTON");
    }
    p.endPos = var->getHomeButton()->getPosition();
    p.startTime = time;
    if (p.endPos.isSameAs(p.startPos)) p.duration = 0;
    else{
        p.duration = (getDistance(startPos, p.endPos)/PACKET_SPEED)*(getRandom(seed)/RANDOM_MAX + 0.5);
        LOG("packet.cpp\tgetPackets() DURATION=" << p.duration);
    }
    p.arrivalTime = p.startTime + secondsToDuration(p.duration);

    auto inputs = p.expression->getInputs(false);
    for(auto& input : inputs){
        p.lastInputsVersions.insert_or_assign(input, -1);
    }

    p.update(*this, true);
    
    LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET CREATED");

    packets.push_back(std::move(p));
    LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET PUSHED");
}

std::vector<Packet> GameState::generatePackets(Button* button, ButtonPosition startPos, Clock::time_point time, uint64_t& seed){
    if (button == nullptr) return {};
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    for(auto& expression : button->getExpressions(*this)){
        addPacketFromAnExpression(expression, packets, startPos, time, seed);
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

std::vector<Packet> GameState::generatePackets(Process* process, ButtonPosition startPos, Clock::time_point time, uint64_t& seed){
    if (process == nullptr) return {};
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    for(auto& expression : process->getExpressions(*this)){
        addPacketFromAnExpression(expression, packets, startPos, time, seed);
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

void GameState::addNewProcessEvent(Process* process, Clock::time_point time){
    if (process == nullptr) return;
    auto it = calendar.begin();

    while (it != calendar.end()){
        if (it->time <= time)
            break;
        it++;
    }

    LOG("game_state.cpp\taddNewProcessEvent() process=" << process->getName());
    CalendarEntry entry = { process, time };

    calendar.insert(it, entry);
}

double GameState::getRealValue(std::string name){
    auto it = variables.find(name);
    if (it != variables.end())
        return it->second.realValue;
    return 0;
}

void GameState::setRealValue(std::string name, double value){
    auto it = variables.find(name);
    if (it != variables.end()){
        it->second.realValue = value;
        updateCurrentInsight();
    }
}


void GameState::updatePackets(){
    for(auto& packet : packets){
        LOG("game_state.cpp\tupdatePackets()");
        packet.update(*this, false);
    }
}

void GameState::setCurrentInsight(std::vector<DisplayLine> insight) {
    LOG("game_state.hpp\tsetCurrentInsight() FUNCTION BEG");
    this->currentInsight = std::move(insight);
    LOG("game_state.hpp\tsetCurrentInsight() FUNCTION END");
};

void GameState::setCurrentInsightable(Insightable* insightable){
    LOG("game_state.hpp\tsetCurrentInsightable() FUNCTION BEG");
    if (insightable == nullptr){
        currentInsight.clear();
        return;
    }
    LOG("game_state.hpp\tsetCurrentInsightable() FUNCTION BEG");
    currentInsightable = insightable;
    LOG("game_state.hpp\tsetCurrentInsightable() CURRENT INSIGHTABLE UPDATED");
    auto insight = currentInsightable->insight(*this, 0);
    setCurrentInsight(insight);
    LOG("game_state.hpp\tsetCurrentInsightable() CURRENT INSIGHT UPDATED");
};

void GameState::updateCurrentInsight(){
    LOG("game_state.hpp\tupdateCurrentInsight() FUNCTION BEG");
    if (currentInsightable == nullptr){
        LOG("game_state.hpp\tupdateCurrentInsight() Insightable is null - clearing");
        currentInsight.clear();
        return;
    }
    auto in = currentInsightable->insight(*this, 0);
    LOG("game_state.hpp\tupdateCurrentInsight() Setting current Insight");
    setCurrentInsight(in);
    LOG("game_state.hpp\tupdateCurrentInsight() Current insight set");
}

bool GameState::isProcessUnlocked(std::string name){
    auto it = processes.find(name);
    if (it == processes.end()) return false;
    return it->second.unlocked;
}

bool GameState::isProcessActive(std::string name){
    auto it = processes.find(name);
    if (it == processes.end()) return false;
    return it->second.active;
}
