#ifndef variable_value_hpp
#define variable_value_hpp
#include <variant>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "insightable.hpp"

class GameState;

enum class Type{
    INT, DOUBLE, BOOL
};

class VariableValue{
    private:
        std::variant<int, double, bool> value;
        Type type;
    
    public:
        VariableValue(int v) : value(v), type(Type::INT) {};
        VariableValue(double v) : value(v), type(Type::DOUBLE) {};
        VariableValue(bool v) : value(v), type(Type::BOOL) {};

        VariableValue add(const VariableValue& other) const;
        VariableValue subtract(const VariableValue& other) const;
        VariableValue multiply(const VariableValue& other) const;
        VariableValue divide(const VariableValue& other) const;
        bool isGreaterThan(const VariableValue& other) const;
        bool isLessThan(const VariableValue& other) const;
        bool isEqualTo(const VariableValue& other) const;

        VariableValue modulo(const VariableValue& other) const;
        VariableValue power(const VariableValue& other) const;
        bool logAnd(const VariableValue& other) const;
        bool logOr(const VariableValue& other) const;
        bool logNot() const;

        int getInt() const;
        double getDouble() const;
        bool getBool() const;
        double getAsDouble() const;
        int getAsInt() const;
        bool getAsBool() const;
        Type getType() const;

        void set(int v);
        void set(double v);
        void set(bool v);
        void set(VariableValue v);
};

class VariableChanges : Insightable{
public:
    std::unordered_map<std::string, double> changes;

    VariableChanges(): changes({}) {};
    VariableChanges add(const VariableChanges& varc) const;
    VariableChanges add(std::string var, double val) const;
    std::vector<DisplayLine> insight([[maybe_unused]] GameState& gameState, [[maybe_unused]] int level) override;
};

#endif