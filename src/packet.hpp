#ifndef packet_hpp
#define packet_hpp
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include "button.hpp"

using Clock = std::chrono::steady_clock;

enum SourceType{
    ButtonS,
    ProcessS
};

struct SourceID{
    SourceType sourceType;
    std::string sourceName;
    size_t termIndex;
    size_t exprIndex;

    bool operator<(const SourceID sid) const{
        return std::tie(sourceType, sourceName, termIndex, exprIndex) <
        std::tie(sid.sourceType, sid.sourceName, sid.termIndex, sid.exprIndex);
    };
};

struct Packet{
    // LOGIC
    bool flag;
    std::string variable;
    Expression expression;
    SourceID source;

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
    Packet(const Packet& other) = default; 
    Packet& operator=(const Packet& other) = default;
};

#endif