#include "game_state.hpp"
#include <iostream>
#include <algorithm>

GameState::GameState() : currentInsight() {
}

double GameState::getTotalScore() const{
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
        LOG("game_state.cpp\tapplyDeltas(changes) ADDED (" << var << " value=" << getVarValue(var) << ")");
    }
    this->updateVariables();
    LOG("game_state.cpp\tapplyDeltas(changes) VARIABLES UPDATED");
}

void GameState::setVarValue(std::string name, double value){
    LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << ") FUNCTION BEG");
    auto it = variables.find(name);
    if (it != variables.end()){
        auto* var = Defs::getVariable(name);
        it->second.value = var->constrain(value);
        LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << " VALUE SET");
    }
    else
        LOG("game_state.cpp\tsetVarValue(name=" << name << ", value=" << value << ") " << name << " NOT FOUND");

    updateEntries();
    updateCurrentInsight();
}  

void GameState::addVarValue(std::string name, double value){
    LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") FUNCTION BEG");

    auto it = variables.find(name);
    if (it != variables.end()){
        setVarValue(name, it->second.value + value);
    }
    else
        LOG("game_state.cpp\taddVarValue(name=" << name << ", value=" << value << ") " << name << " NOT FOUND");
}

void GameState::updateEntries(){
    updateVariables();
    updateButtons();
    updateProcesses();
}

void GameState::updateVariables(){
    for (auto& [name, entry] : variables){
        Variable* var = Defs::getVariable(name);
        if (var != nullptr){
            if (!entry.isUnlocked() && var->getUnlockCondition().evaluate(*this)){
                LOG("game_state.cpp\tupdateVariables() " << name << " UNLOCKED");
                entry.unlock();
            }
        }
    }
}

void GameState::updateButtons(){
    for (auto& [name, entry] : buttons){
        Button* btn = Defs::getButton(name);

        if (btn == nullptr) continue;
        
        if (!entry.isUnlocked() && btn->isUnlocked(*this))
            entry.unlock();
    }
}

void GameState::updateProcesses(){
    std::cout << "updateProcesses fun\n";
    for (auto& [name, entry] : processes){
        Process* proc = Defs::getProcess(name);

        if (proc == nullptr) continue;

        if (!entry.isUnlocked() && proc->isUnlocked(*this))
            entry.unlock();

        if (!entry.isUnlocked()) return;
        
        std::cout << "updateProcesses unlocked\n";

        if (entry.isActive()){
            if (proc->isEndConditionMet(*this)){
                entry.deactivate();
            }
        }
        else{
            std::cout << "not active but cond?\n";
            if (proc->isStartConditionMet(*this) && !proc->isEndConditionMet(*this)){
                entry.activate();
                std::cout << "process activated??" << std::endl;
                addNewProcessEvent(proc, proc->getInterval(*this));
            }
        }
    }
}

bool GameState::isVariableUnlocked(std::string name) const{
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).isUnlocked();
}

void GameState::blockVariable(std::string name){
    if (variables.find(name) != variables.end())
        variables.at(name).incrementBlockCounter();
}

void GameState::unblockVariable(std::string name){
    if (variables.find(name) != variables.end())
        variables.at(name).decrementBlockCounter();
}

bool GameState::isVariableBlocked(std::string name) const{
    if (variables.find(name) == variables.end()) return false;
    return variables.at(name).isBlocked();
}

int GameState::getVariableBlockCounter(std::string name){
    auto it = variables.find(name);
    if (it != variables.end()) return it->second.getBlockedCounter();
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

void GameState::addProcess(Process* process){
    ProcessEntry entry;
    std::string name = process->getName();
    std::cout << "new process " << name << "added to gamestate\n";
    processes.insert({name, entry});
}

void GameState::addProcesss(){
    for(auto& [name, proc] : Defs::procs){
        addProcess(&proc);
    }
    updateProcesses();
}

void GameState::init(){
    addVariables();
    addButtons();
    addProcesss();
}

bool GameState::isButtonUnlocked(std::string name) const{
    if (buttons.find(name) == buttons.end()) return false;
    return buttons.at(name).isUnlocked();
}

void GameState::printUnlocked(){
    for (auto& [name, entry] : variables){
        if (entry.isUnlocked())
            std::cout << "unlocked: " << name << std::endl;
    }
}

double GameState::getCurrentrandom() const{
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

void GameState::sendPacket(Packet packet, bool update = true) {
    // packets with lowest arrivalTime at the end
    auto it = packets.begin();

    while (it != packets.end()){
        if (it->arrivalTime <= packet.arrivalTime)
            break;
        it++;
    }

    LOG("game_state.cpp\taddPacket() variable=" << packet.variable);
    std::string name = packet.variable;
    for(auto lock : packet.expression.variableLocks){
        blockVariable(lock);
    }

    packets.insert(it, std::move(packet));

    it = packets.begin();
    while (it != packets.end()){
        std::cout << it->arrivalTime << "\n";
        it++;
    }

    if (update){
        updatePacketsAndRealValues();
        //updateCurrentInsight();
    }
}

void GameState::sendPackets(std::vector<Packet>& packets) {
    for(auto& packet : packets){
        sendPacket(packet, false);
    }
    packets.clear();
    updatePacketsAndRealValues();
    //updateCurrentInsight();
}

void GameState::addPacketFromAnExpression(const Expression& expression, std::vector<Packet>& packets, ButtonPosition startPos, SourceID source){
    addPacketFromAnExpression(expression, packets, startPos, inGameTime, seed, source);
}

void GameState::addPacketFromAnExpression(const Expression& expression, std::vector<Packet>& packets, ButtonPosition startPos, double time, uint64_t& seed, SourceID source){
    auto outputs = expression.expr->getOutputs(true);
    if (outputs.empty()) return;
    std::string name = *outputs.begin();

    std::unique_ptr<Node> exprNode = expression.expr->getPacketExpression(*this, true);
    

    LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << "?");
    Variable* var = Defs::getVariable(name);

    Expression expr(std::move(exprNode), expression.variableLocks);

    Packet p(std::move(expr));
    p.variable = name;
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
    p.arrivalTime = p.startTime + p.duration;
    p.source = source;
    
    LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET CREATED");

    auto it = packets.begin();

    while (it != packets.end()){
        if (it->arrivalTime <= p.arrivalTime)
            break;
        it++;
    }

    packets.insert(it, std::move(p));

    LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET PUSHED");
}

std::vector<Packet> GameState::generatePackets(Button* button){
    return generatePackets(button, button->getPosition(), inGameTime, seed);
}

std::vector<Packet> GameState::generatePackets(Process* process, ButtonPosition startPos){
    return generatePackets(process, startPos, inGameTime, seed);
}

std::vector<Packet> GameState::generatePackets(Button* button, ButtonPosition startPos, double time, uint64_t& seed){
    if (button == nullptr) return {};
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    int termID = 0; int exprID = 0;
    for(auto& term : button->getTerms()){
        if (!term->isActive(*this)) {
            termID++; continue;
        }
        for(auto& expression : term->getExpressions()){
            SourceID source = { SourceType::ProcessS, button->getName(), termID, exprID };
            addPacketFromAnExpression(expression, packets, startPos, time, seed, source);
            exprID++;
        }
        termID++; exprID = 0;
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

std::vector<Packet> GameState::generatePackets(Process* process, ButtonPosition startPos, double time, uint64_t& seed){
    if (process == nullptr) return {};
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    int termID = 0; int exprID = 0;
    for(auto& term : process->getTerms()){
        if (!term->isActive(*this)) {
            termID++; continue;
        }
        for(auto& expression : term->getExpressions()){
            SourceID source = { SourceType::ProcessS, process->getName(), termID, exprID };
            addPacketFromAnExpression(expression, packets, startPos, time, seed, source);
            exprID++;
        }
        termID++; exprID = 0;
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

void GameState::addNewProcessEvent(Process* process, double seconds){
    if (process == nullptr) return;
    auto it = calendar.begin();

    auto time = getInGameTime()+seconds;

    while (it != calendar.end()){
        if (it->time <= time)
            break;
        it++;
    }

    LOG("game_state.cpp\taddNewProcessEvent() process=" << process->getName() << " time=" << inGameTime+seconds);
    CalendarEntry entry = { process, CalendarEntryType::ProcessTick, time };

    calendar.insert(it, entry);
}

double GameState::getRealValue(std::string name) const{
    auto it = variables.find(name);
    if (it != variables.end())
        return it->second.realValue;
    return 0;
}

void GameState::setRealValue(std::string name, double value){
    auto it = variables.find(name);
    if (it != variables.end()){
        it->second.realValue = value;
    }
}

void GameState::clearRealValues(){
    for(auto& [name, entry] : variables){
        entry.realValue = entry.value;
    }
}

void GameState::updatePacketsAndRealValues(){
    clearRealValues();
    auto it = packets.end();
    while(it != packets.begin()){
        it--;
        double newValue = it->expression.evaluate(*this, true);
        double oldValue = getRealValue(it->variable);
        setRealValue(it->variable, newValue);
        LOG("game_state.cpp\tupdatePacketsAndRealValues() new Real Value for " << it->variable << ": " << newValue << " (" << oldValue << ")");
        it->update(newValue-oldValue);
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

bool GameState::isProcessUnlocked(std::string name) const{
    auto it = processes.find(name);
    if (it == processes.end()) return false;
    return it->second.isUnlocked();
}

bool GameState::isProcessActive(std::string name) const{
    auto it = processes.find(name);
    if (it == processes.end()) return false;
    return it->second.isActive();
}

// GameState::GameState(const GameState& gs){
//     variables = gs.variables;
//     processes = gs.processes;
//     buttons = gs.buttons;
//     currentSeed = gs.currentSeed;
//     inGameTime = gs.inGameTime;
//     forcedRandom = -1;

//     probabilities = gs.probabilities;
//     currentTerm = gs.currentTerm;
//     currentIndex = gs.currentIndex;
//     currentInsightable = nullptr;

//     packets = gs.packets;
//     calendar = gs.calendar;
//     seed = gs.seed;
// };

std::map<SourceID, VariableChanges> GameState::predict(Button* button) const{
    GameState copy(*this);
    copy.currentInsightable = nullptr;
    auto packets = copy.generatePackets(button);

    std::map<SourceID, VariableChanges> effects;
    
    while(!packets.empty()){
        double calTime = copy.calendar.empty() ? INFINITY : copy.calendar.back().time;
        double oldPTime = copy.packets.empty() ? INFINITY : copy.packets.back().arrivalTime;
        double newPTime = packets.back().arrivalTime;

        if (calTime < oldPTime && calTime < newPTime){
            auto& entry = copy.calendar.back();

            std::cout << "caltime\n";

            auto packets = copy.generatePackets(entry.process, ButtonPosition(0, 5));
            copy.sendPackets(packets);
            copy.updateProcesses();
            if (copy.isProcessActive(entry.process->getName())){
                double interval = entry.process->getInterval(copy);
                copy.addNewProcessEvent(entry.process, interval);
            }
            copy.calendar.pop_back();
        }
        else if (oldPTime <= newPTime){
            auto& entry = copy.packets.back();

            std::cout << "oldptime\n";

            double newValue = entry.expression.evaluate(copy);
            for(auto lock : entry.expression.variableLocks){
                copy.unblockVariable(lock);
            }
            copy.setVarValue(entry.variable, newValue);
            copy.packets.pop_back();
        }
        else{
            auto& entry = packets.back();
            
            std::cout << "new packet var: " << entry.variable << std::endl; 
            double newValue = entry.expression.evaluate(copy);
            double delta = newValue - copy.getVarValue(entry.variable);
            effects.insert_or_assign(entry.source, VariableChanges(entry.variable, delta));
            for(auto lock : entry.expression.variableLocks){
                copy.unblockVariable(lock);
            }
            copy.setVarValue(entry.variable, newValue);
            packets.pop_back();
        }
    }

    return effects;
}
