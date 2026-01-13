#include "variable.hpp"
#include <cmath>
#include <algorithm>
#include "expressiontree.hpp"

double ScoreParams::getScore(double val){
    if (polarity == Polarity::Neutral)
        return 0.0;
        
    val -= offset;

    if (polarity == Polarity::Inverted)
        val = -val;
    if (polarity == Polarity::Absolute)
        val = abs(val);

    if (val <= 0) return 0.f;

    double logValue = log(val)/log(knee);

    if (logValue <= 0) return 0.f;
    return logValue*100;
}

ScoreParams getScoreParams(double knee, double offset, Polarity polarity) {
    return {knee, offset, polarity};
}