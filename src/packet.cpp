#include "packet.hpp"
#include "variable.hpp"
#include "button.hpp"

std::vector<Packet> getPackets(GameState& gameState, Button* button, ButtonPosition startPos, Clock::time_point time, uint64_t& seed){
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    for(auto& expression : button->getExpressions(gameState)){
        auto outputs = expression->getOutputs(true);
        if (outputs.empty()) continue;
        std::string name = *outputs.begin();

        std::unique_ptr<Node> expr = expression->getPacketExpression(gameState);

        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << "?");
        Variable* var = Defs::getVariable(name);

        Packet p;
        p.variable = name;
        p.expression = std::move(expr);
        p.startPos = startPos;
        if (var->getHomeButton() == nullptr){
            LOG("packet.cpp\tgetPackets() NAME=" << name << " HAVE AN INVALID HOME BUTTON");
        }
        p.endPos = var->getHomeButton()->getPosition();
        p.startTime = time;
        if (p.endPos.isSameAs(p.startPos)) p.duration = 0;
        else{
            p.duration = (getDistance(startPos, p.endPos)/PACKET_SPEED)*(getRandom(seed)/RANDOM_MAX + 0.5);
            LOG("packet.cpp\tgetPackets() DURATION=" << p.duration);
        }
        p.arrivalTime = p.startTime + secondsToDuration(p.duration);
        p.lastVersion = -1;

        p.update(gameState);
        
        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << "?" << " PACKET CREATED");

        packets.push_back(p);
        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << "?" << " PACKET PUSHED");
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

void Packet::update(GameState& gameState){
    int version = gameState.getVarVersion(variable);
    if (lastVersion < version){
        lastVersion = version;
        double currentValue = gameState.getVarValueAsDouble(variable);
        double expressionEvaluation = expression->evaluate(gameState).getAsDouble();
        double delta = expressionEvaluation - currentValue;
        // update color and radius
    }
}

double Packet::getProgress(Clock::time_point time) const{
    double timePassed = durationToSeconds(time - startTime);
    if (timePassed > duration) return 1;
    return timePassed/duration;
}