#include "packet.hpp"
#include "variable.hpp"
#include "button.hpp"

std::vector<Packet> getPackets(GameState& gameState, Button* button, ButtonPosition startPos, Clock::time_point time, uint64_t& seed){
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    for(auto& expression : button->getExpressions(gameState)){
        auto outputs = expression.expr->getOutputs(true);
        if (outputs.empty()) continue;
        std::string name = *outputs.begin();

        std::unique_ptr<Node> expr = expression.expr->getPacketExpression(gameState, true);
        

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

        p.update(gameState, true);
        
        LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET CREATED");

        packets.push_back(std::move(p));
        LOG("packet.cpp\tgetPackets() NAME=" << p.variable << " DELTA=" << "?" << " PACKET PUSHED");
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}

void Packet::update(GameState& gameState, bool forced){
    bool toUpdate = false;
    for(auto& [variable, version] : lastInputsVersions){
        int currentVersion = gameState.getVarVersion(variable);
        LOG("packet.cpp\tupdate() variable=" << variable << " version=" << version << " currentVersion=" << currentVersion);
        if (version < currentVersion){
            version = currentVersion;
            toUpdate = true;
        }
    }

    LOG("packet.cpp\tupdate() toBeUpdated=" << toUpdate << " forced=" << forced);
    if (toUpdate || forced){
        double currentValue = gameState.getVarValueAsDouble(variable);
        double expressionEvaluation = expression->evaluate(gameState);
        //LOG("packet.cpp\tupdate() exprEvaluation=" << expressionEvaluation);
        double delta = expressionEvaluation - currentValue;
        // update color and radius
        if (delta > 0) color = GREEN;
        else if (delta < 0) color = RED;
        else color = WHITE;

        radius = 20;
    }
}

double Packet::getProgress(Clock::time_point time) const{
    double timePassed = durationToSeconds(time - startTime);
    if (timePassed > duration) return 1;
    return timePassed/duration;
}