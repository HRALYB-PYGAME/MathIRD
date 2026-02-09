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
    double startTime;
    double duration;
    double arrivalTime;

    // POSITION
    ButtonPosition startPos;
    ButtonPosition endPos;

    double getProgress(double time) const;
    void update(double delta);

    Packet(Expression expr): expression(std::move(expr)) {};
};

#endif