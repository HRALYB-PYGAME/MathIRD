#ifndef packet_hpp
#define packet_hpp
#include <string>
#include "utils.hpp"
#include <vector>
#include "variable_value.hpp"
#include <chrono>

using Clock = std::chrono::steady_clock;

struct Packet{
    std::string variable;
    double delta;

    Clock::time_point startTime;
    double duration;
    Clock::time_point arrivalTime;

    ButtonPosition startPos;
    ButtonPosition endPos;

    double getProgress(Clock::time_point time) const;
};

std::vector<Packet> getPackets(VariableChanges deltas, ButtonPosition startPos, Clock::time_point time);

#endif