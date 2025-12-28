#include "variable.hpp"
#include <cmath>
#include <algorithm>

double Variable::getScore(){
    if (this->scoreParams.polarity == Polarity::Neutral)
        return 0.0;
        
    double value = this->value.getAsDouble();
    value -= this->scoreParams.offset;

    if (this->scoreParams.polarity == Polarity::Inverted)
        value = -value;
    if (this->scoreParams.polarity == Polarity::Absolute)
        value = abs(value);

    if (value <= 0) return 0.f;

    double logValue = log(value)/log(this->scoreParams.knee);

    if (logValue <= 0) return 0.f;
    return logValue*100;
}

void Variable::add(int v){
    if (this->value.getType() == Type::BOOL) return;
    if (this->value.getType() == Type::INT)
        this->set(this->value.getInt() + v);
    if (this->value.getType() == Type::DOUBLE)
        this->set(this->value.getDouble() + (double)v);
}
void Variable::add(double v){
    if (this->value.getType() != Type::DOUBLE) return;
    this->set(this->value.getDouble() + v);
}
void Variable::toggle(){
    if (this->value.getType() != Type::BOOL) return;
    this->set(!this->value.getBool());
}

ScoreParams getScoreParams(double knee, double offset, Polarity polarity) {
    return {knee, offset, polarity};
}