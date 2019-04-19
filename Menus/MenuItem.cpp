/*
 * MenuItem.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "MenuItem.h"

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>

MenuItem::MenuItem(std::string name, command c) :
    _optionType(OptionType::NONE),
    _name(name),
    _fn(c),
    _value(0),
    _max(0) {

}

MenuItem::MenuItem(std::string name, command c, int value, int max,
                   OptionType type) :
    _optionType(type),
    _name(name),
    _fn(c),
    _value(value),
    _max(max) {

}

MenuItem::~MenuItem() {
}

command MenuItem::getFn() const {
    return _fn;
}

const std::string &MenuItem::getName() const {
    return _name;
}

MenuItem::OptionType MenuItem::getOptionType() const {
    return _optionType;
}

int MenuItem::getMax() const {
    return _max;
}

int MenuItem::getValue() const {
    return _value;
}

void MenuItem::increase() {
    ++_value;
    if (_value > _max) {
        _value = 0;
    }
}

void MenuItem::decrease() {
    --_value;
    if (_value < 0) {
        _value = _max;
    }
}

void MenuItem::setValue(int value) {
    _value = value;
}

const std::string MenuItem::getOptionString() const {
    std::ostringstream os;
    switch (_optionType) {
        case OptionType::TOGGLE:
            if (_value != 0) {
                return "ON";
            } else {
                return "OFF";
            }
            break;
        case OptionType::SLIDER: os << _value << '/' << _max;
            return os.str();
            break;
        case OptionType::PLAYER: os << "P" << _value + 1;
            return os.str();
            break;
        default: return "";
            break;
    }
}
