/*
 * ScorePanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "Popup.h"
#include <sstream>

Popup::Popup() :
				_x(0),
				_y(0),
				_value(0),
				_ticksToLive(0),
				_ticksLived(0),
				_alive(true) {
}

Popup::Popup(int x, int y, int value, int lifetime) :
				_x(x),
				_y(y),
				_value(value),
				_ticksToLive(lifetime),
				_ticksLived(0),
				_alive(true){
	initDigits();
}

void Popup::tick() {
	if (_ticksLived <= 5) {
		--_y;
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

