#include "packet.hpp"

void Packet::update(double delta){
    std::cout << "NEW PACKET DELTA: " << delta << " previous: " << currDelta << "--------------------------------------------------------------------------------\n";
    currDelta = delta;
    if (delta > 0) color = GREEN;
    else if (delta < 0) color = RED;
    else color = WHITE;

    radius = 20;
}

double Packet::getProgress(double time) const{
    double timePassed = time - startTime;
    if (timePassed > duration) return 1;
    return timePassed/duration;
}