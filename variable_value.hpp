#ifndef variable_value_hpp
#define variable_value_hpp
#include <variant>
#include <string>

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

        const int getInt() const;
        const double getDouble() const;
        const bool getBool() const;
        const double getAsDouble() const;
        const double getAsInt() const;
        const Type getType() const;

        void set(int v);
        void set(double v);
        void set(bool v);
};

#endif