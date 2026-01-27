#include "packet.hpp"
#include "variable.hpp"
#include "button.hpp"

std::vector<Packet> getPackets(VariableChanges deltas, ButtonPosition startPos, Clock::time_point time){
    LOG("packet.cpp\tgetPackets() FUNCTION BEG");
    std::vector<Packet> packets;
    for(auto [name, delta] : deltas.changes){
        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << delta.rand);
        Variable* var = Defs::getVariable(name);
        Packet p;
        p.variable = name;
        p.delta = delta.rand;
        p.startPos = startPos;
        if (var->getHomeButton() == nullptr){
            LOG("packet.cpp\tgetPackets() NAME=" << name << " HAVE AN INVALID HOME BUTTON");
        }
        p.endPos = var->getHomeButton()->getPosition();
        p.startTime = time;
        p.duration = getDistance(startPos, p.endPos)/PACKET_SPEED;
        p.arrivalTime = p.startTime + to_duration(p.duration);
        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << delta.rand << " PACKET CREATED");

        packets.push_back(p);
        LOG("packet.cpp\tgetPackets() NAME=" << name << " DELTA=" << delta.rand << " PACKET PUSHED");
    }
    LOG("packet.cpp\tgetPackets() FUNCTION END");
    return packets;
}