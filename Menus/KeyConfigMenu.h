/*
 * KeyConfigMenu.h
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#ifndef MENUS_KEYCONFIGMENU_H_
#define MENUS_KEYCONFIGMENU_H_

#include "../Config/KeyboardControllerConfig.h"
#include "Menu.h"

class OptionsMenuState;

class KeyConfigMenu : public Menu {
public:
	KeyConfigMenu(OptionsMenuState&, int player);
	virtual ~KeyConfigMenu();

	void recieveKey(int);

	void render() const;
private:
	OptionsMenuState& _state;
	int _player;
	int* _activeKey;

	KeyboardControllerConfig _newKeyConfig;
};

#endif /* MENUS_KEYCONFIGMENU_H_ */
