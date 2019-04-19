/*
 * ScorePanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "Popup.h"

#include <sstream>

Popup::Popup() :
    _value(0) {
}

Popup::Popup(int x, int y, int value, int lifetime) :
    Particle(x, y, 0, -1, lifetime),
    _value(value) {
    initDigits();
}

void Popup::tick() {
    if (_ticksLived <= 5) {
        _x += _vx;
        _y += _vy;
    }
    ++_ticksLived;
    if (_ticksLived >= _ticksToLive) {
        _alive = false;
    }
}

void Popup::initDigits() {
    std::ostringstream os;
    os << _value;
    _digits = os.str();
}

Popup::~Popup() {
}

