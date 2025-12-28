#ifndef variable_hpp
#define variable_hpp

#include <string>
#include <variant>
#include "variable_value.hpp"

enum class Polarity{
    Normal,
    Inverted,
    Absolute,
    Neutral
};

struct ScoreParams{
    double knee;
    double offset;
    Polarity polarity;
};

ScoreParams getScoreParams(double knee, double offset, Polarity polarity);

class Variable {
    private:
        std::string name;
        ScoreParams scoreParams;
        VariableValue value;
    public:
        Variable(std::string name, VariableValue value, ScoreParams scoreParams): 
            name(std::move(name)), value(std::move(value)), scoreParams(scoreParams){};
        double getScore();
        VariableValue getValue(){ return value; };
        void set(int v){ value.set(v); };
        void set(double v){ value.set(v); };
        void set(bool v){ value.set(v); };
        void set(VariableValue v){ value = std::move(v);};

        void add(int v);
        void add(double v);
        void toggle();
};

#endif