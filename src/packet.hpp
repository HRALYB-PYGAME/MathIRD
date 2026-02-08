#ifndef packet_hpp
#define packet_hpp
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "button.hpp"

using Clock = std::chrono::steady_clock;

struct Packet{
    // LOGIC
    std::string variable;
    Expression expression;

    // VISUALS
    double radius;
    Color color;

    double currDelta = 0;

    // TIMING
    Clock::time_point startTime;
    double duration;
    Clock::time_point arrivalTime;

    // POSITION
    ButtonPosition startPos;
    ButtonPosition endPos;

    double getProgress(Clock::time_point time) const;
    void update(double delta);

    Packet(Expression expr): expression(std::move(expr)) {};
};

#endif