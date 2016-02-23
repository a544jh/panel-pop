/*
 * OptionsState.h
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#ifndef STATES_OPTIONSMENUSTATE_H_
#define STATES_OPTIONSMENUSTATE_H_

#include "../Config/KeyboardControllerConfig.h"
#include "../Rendering/Renderer.h"
#include "State.h"
#include <list>

class Menu;

class OptionsMenuState: public State, public Renderer{
public:
	OptionsMenuState();
	virtual ~OptionsMenuState();
	void tick();
	SDL_Texture* render();
	void goBack();

private:
	std::list<Menu*> _menuStack;
	KeyboardControllerConfig _p1keys, _p2keys;
	Menu* _currentMenu;
};

#endif /* STATES_OPTIONSMENUSTATE_H_ */
