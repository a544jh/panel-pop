/*
 * MenuItem.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "MenuItem.h"

MenuItem::MenuItem(std::string name, command c) :
				_changable(false),
				_name(name),
				_fn(c),
				_value(0),
				_max(0) {

}

MenuItem::MenuItem(std::string name, command c, int value, int max) :
				_changable(true),
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

const std::string& MenuItem::getName() const {
	return _name;
}

bool MenuItem::isChangable() const {
	return _changable;
}

int MenuItem::getMax() const {
	return _max;
}

int MenuItem::getValue() const {
	return _value;
}

void MenuItem::increase() {
}

void MenuItem::decrease() {
}

void MenuItem::setValue(int value) {
	_value = value;
}
