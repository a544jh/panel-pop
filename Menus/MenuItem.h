/*
 * MenuItem.h
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#ifndef MENUS_MENUITEM_H_
#define MENUS_MENUITEM_H_

#include <string>
#include <functional>

class Menu;

typedef std::function<void()> command;

class MenuItem {
public:
	MenuItem(std::string, command);
	virtual ~MenuItem();
	command getFn() const;
	const std::string& getName() const;

private:
	std::string _name;
	command _fn;
};

#endif /* MENUS_MENUITEM_H_ */
