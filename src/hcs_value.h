#pragma once

#include <string>
#include <memory>
#include <unordered_map>

class HCSValue {
public:
    enum class Type { Null, Integer, Double, String, Bool, Blank };
    
    Type type;
    
    // Store values as strings or specific types
    long long intVal = 0;
    double doubleVal = 0.0;
    std::string stringVal;
    bool boolVal = false;
    
    // Constructors
    HCSValue() : type(Type::Null) {}
    explicit HCSValue(long long i) : type(Type::Integer), intVal(i) {}
    explicit HCSValue(double d) : type(Type::Double), doubleVal(d) {}
    explicit HCSValue(const std::string& s) : type(Type::String), stringVal(s) {}
    explicit HCSValue(bool b) : type(Type::Bool), boolVal(b) {}
    
    static HCSValue blank() {
        HCSValue v;
        v.type = Type::Blank;
        return v;
    }
    
    // Type conversion
    long long toInteger() const;
    double toDouble() const;
    std::string toString() const;
    bool toBoolean() const;
    
    // Operators
    HCSValue operator+(const HCSValue& other) const;
    HCSValue operator-(const HCSValue& other) const;
    HCSValue operator*(const HCSValue& other) const;
    HCSValue operator/(const HCSValue& other) const;
    HCSValue operator%(const HCSValue& other) const;
    
    bool operator==(const HCSValue& other) const;
    bool operator!=(const HCSValue& other) const;
    bool operator<(const HCSValue& other) const;
    bool operator>(const HCSValue& other) const;
    bool operator<=(const HCSValue& other) const;
    bool operator>=(const HCSValue& other) const;
    
    // Assignment
    HCSValue& operator=(const HCSValue& other);
};

using HCSValuePtr = std::shared_ptr<HCSValue>;
