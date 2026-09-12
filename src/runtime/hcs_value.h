#pragma once

#include <string>
#include <variant>

namespace runtime {

class HCSValue {
public:
    using Value = std::variant<std::monostate, int64_t, double, std::string, bool>;

    HCSValue() = default; // blank -> monostate
    explicit HCSValue(int64_t i) : value_(i) {}
    explicit HCSValue(double d) : value_(d) {}
    explicit HCSValue(const std::string& s) : value_(s) {}
    explicit HCSValue(bool b) : value_(b) {}

    bool isBlank() const { return std::holds_alternative<std::monostate>(value_); }
    bool isInt() const { return std::holds_alternative<int64_t>(value_); }
    bool isDouble() const { return std::holds_alternative<double>(value_); }
    bool isString() const { return std::holds_alternative<std::string>(value_); }
    bool isBool() const { return std::holds_alternative<bool>(value_); }

    const Value& value() const { return value_; }

private:
    Value value_;
};

} // namespace runtime
