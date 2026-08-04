#pragma once
#include "exceptions.hpp"

template <typename T>
class Option
{
private:
    bool hasValue;
    T value;
public:
    Option() : hasValue(false) {}
    Option(const T &val) : hasValue(true), value(val) {}

    bool HasValue() const {return hasValue;}

    T GetValue() const
    {
        if (!hasValue)
            throw InvalidArgument("Option не содержит значения");
        return value;
    }

    T GetValueOrDefault(const T &def) const
    {
        return hasValue ? value : def;
    }
};
