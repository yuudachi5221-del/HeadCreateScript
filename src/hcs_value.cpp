#include "hcs_value.h"
#include <stdexcept>
#include <cmath>
#include <sstream>

long long HCSValue::toInteger() const {
    switch (type) {
        case Type::Integer:
            return intVal;
        case Type::Double:
            return static_cast<long long>(doubleVal);
        case Type::String:
            try {
                return std::stoll(stringVal);
            } catch (...) {
                return 0;
            }
        case Type::Bool:
            return boolVal ? 1 : 0;
        case Type::Blank:
        case Type::Null:
            return 0;
    }
    return 0;
}

double HCSValue::toDouble() const {
    switch (type) {
        case Type::Integer:
            return static_cast<double>(intVal);
        case Type::Double:
            return doubleVal;
        case Type::String:
            try {
                return std::stod(stringVal);
            } catch (...) {
                return 0.0;
            }
        case Type::Bool:
            return boolVal ? 1.0 : 0.0;
        case Type::Blank:
        case Type::Null:
            return 0.0;
    }
    return 0.0;
}

std::string HCSValue::toString() const {
    switch (type) {
        case Type::Integer:
            return std::to_string(intVal);
        case Type::Double: {
            std::ostringstream oss;
            oss << doubleVal;
            return oss.str();
        }
        case Type::String:
            return stringVal;
        case Type::Bool:
            return boolVal ? "true" : "false";
        case Type::Blank:
            return "blank";
        case Type::Null:
            return "";
    }
    return "";
}

bool HCSValue::toBoolean() const {
    switch (type) {
        case Type::Integer:
            return intVal != 0;
        case Type::Double:
            return doubleVal != 0.0;
        case Type::String:
            return !stringVal.empty();
        case Type::Bool:
            return boolVal;
        case Type::Blank:
        case Type::Null:
            return false;
    }
    return false;
}

HCSValue HCSValue::operator+(const HCSValue& other) const {
    // If either is string, concatenate
    if (type == Type::String || other.type == Type::String) {
        return HCSValue(toString() + other.toString());
    }
    
    // If either is double, do floating point
    if (type == Type::Double || other.type == Type::Double) {
        return HCSValue(toDouble() + other.toDouble());
    }
    
    // Integer addition
    return HCSValue(toInteger() + other.toInteger());
}

HCSValue HCSValue::operator-(const HCSValue& other) const {
    if (type == Type::Double || other.type == Type::Double) {
        return HCSValue(toDouble() - other.toDouble());
    }
    return HCSValue(toInteger() - other.toInteger());
}

HCSValue HCSValue::operator*(const HCSValue& other) const {
    if (type == Type::Double || other.type == Type::Double) {
        return HCSValue(toDouble() * other.toDouble());
    }
    return HCSValue(toInteger() * other.toInteger());
}

HCSValue HCSValue::operator/(const HCSValue& other) const {
    if (type == Type::Double || other.type == Type::Double) {
        double divisor = other.toDouble();
        if (divisor == 0.0) throw std::runtime_error("Division by zero");
        return HCSValue(toDouble() / divisor);
    }
    long long divisor = other.toInteger();
    if (divisor == 0) throw std::runtime_error("Division by zero");
    return HCSValue(toInteger() / divisor);
}

HCSValue HCSValue::operator%(const HCSValue& other) const {
    long long divisor = other.toInteger();
    if (divisor == 0) throw std::runtime_error("Modulo by zero");
    return HCSValue(toInteger() % divisor);
}

bool HCSValue::operator==(const HCSValue& other) const {
    if (type != other.type) {
        // Type coercion for comparison
        return toString() == other.toString();
    }
    switch (type) {
        case Type::Integer:
            return intVal == other.intVal;
        case Type::Double:
            return std::abs(doubleVal - other.doubleVal) < 1e-9;
        case Type::String:
            return stringVal == other.stringVal;
        case Type::Bool:
            return boolVal == other.boolVal;
        case Type::Blank:
        case Type::Null:
            return true;
    }
    return false;
}

bool HCSValue::operator!=(const HCSValue& other) const {
    return !(*this == other);
}

bool HCSValue::operator<(const HCSValue& other) const {
    if (type == Type::String || other.type == Type::String) {
        return toString() < other.toString();
    }
    if (type == Type::Double || other.type == Type::Double) {
        return toDouble() < other.toDouble();
    }
    return toInteger() < other.toInteger();
}

bool HCSValue::operator>(const HCSValue& other) const {
    return other < *this;
}

bool HCSValue::operator<=(const HCSValue& other) const {
    return *this < other || *this == other;
}

bool HCSValue::operator>=(const HCSValue& other) const {
    return other <= *this;
}

HCSValue& HCSValue::operator=(const HCSValue& other) {
    type = other.type;
    intVal = other.intVal;
    doubleVal = other.doubleVal;
    stringVal = other.stringVal;
    boolVal = other.boolVal;
    return *this;
}
