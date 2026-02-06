#include "packet.hpp"
#include "game_state.hpp"

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