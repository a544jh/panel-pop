/*
 * OptionsMenu.h
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#ifndef MENUS_OPTIONSMENU_H_
#define MENUS_OPTIONSMENU_H_

#include "Menu.h"

class OptionsMenu: public Menu {
public:
	OptionsMenu();
	virtual ~OptionsMenu();
	void render() const;
};

#endif /* MENUS_OPTIONSMENU_H_ */
