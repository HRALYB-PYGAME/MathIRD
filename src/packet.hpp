#ifndef packet_hpp
#define packet_hpp
#include <string>
#include "utils.hpp"
#include <vector>
#include "variable_value.hpp"
#include "expressiontree.hpp"
#include <chrono>

using Clock = std::chrono::steady_clock;

struct Packet{
    // LOGIC
    std::string variable;
    std::unique_ptr<Node> expression;
    double lastVersion;

    // VISUALS
    double radius;
    Color color;

    // TIMING
    Clock::time_point startTime;
    double duration;
    Clock::time_point arrivalTime;

    // POSITION
    ButtonPosition startPos;
    ButtonPosition endPos;

    double getProgress(Clock::time_point time) const;
    void update(GameState& gameState);
};

std::vector<Packet> getPackets(GameState& gameState, Button* button, ButtonPosition startPos, Clock::time_point time, uint64_t& seed);

#endif