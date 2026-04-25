#pragma once

#include <limits>
#include <set>
#include <string>
#include <vector>

namespace Charcoal {

struct Parameter;

struct Command {
    const std::string name;
    const std::vector<Parameter *> subnode;

    void (*callback)(void **args) = nullptr;
};

class Parameter {
    virtual bool validate(const std::string &value);
};

class IntegerParameter : public Parameter {
    std::set<int> acceptable_values;
    int min = std::numeric_limits<int>::min();
    int max = std::numeric_limits<int>::max();
};

class BoolParameter : public Parameter {
    bool validate(const std::string &value) {
        if ()
    }
};

class FloatParameter : public Parameter {
    std::set<float> acceptable_values;
    float min = std::numeric_limits<float>::min();
    float max = std::numeric_limits<float>::max();
};

class StringParameter : public Parameter {
    std::set<std::string> acceptable_values;
};

} // namespace Charcoal
