/*
 * MenuItem.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "MenuItem.h"

MenuItem::MenuItem(std::string name, command c) :
				_name(name),
				_fn(c) {

}

MenuItem::~MenuItem() {
}

command MenuItem::getFn() const {
	return _fn;
}

const std::string& MenuItem::getName() const {
	return _name;
}
