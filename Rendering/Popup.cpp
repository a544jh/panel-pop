/*
 * ScorePanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "Popup.h"
#include <sstream>

Popup::Popup() :
		_alive(true) {
}

Popup::Popup(int x, int y, int value, int lifetime) :
		_alive(true), _x(x), _y(y), _value(value), _ticksToLive(lifetime), _ticksLived(
				0) {
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
	// TODO Auto-generated destructor stub
}

