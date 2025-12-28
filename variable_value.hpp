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

        VariableValue add(VariableValue& other);
        VariableValue subtract(VariableValue& other);
        VariableValue multiply(VariableValue& other);
        VariableValue divide(VariableValue& other);

        int getInt();
        double getDouble();
        bool getBool();
        double getAsDouble();
        double getAsInt();
        Type getType();

        void set(int v);
        void set(double v);
        void set(bool v);
};

#endif