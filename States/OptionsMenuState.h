/*
 * OptionsState.h
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#ifndef STATES_OPTIONSMENUSTATE_H_
#define STATES_OPTIONSMENUSTATE_H_

#include "../Rendering/Renderer.h"
#include "State.h"
#include <list>
#include "../Config/KeyConfig.h"

class Menu;

class OptionsMenuState: public State, public Renderer{
public:
	OptionsMenuState();
	virtual ~OptionsMenuState();
	void tick();
	SDL_Texture* render();
	void goBack();
	void configurePlayerKeys(int);

private:
	std::list<Menu*> _menuStack;
	KeyConfig _p1keys, _p2keys;
	Menu* _currentMenu;
	void pushMenu(Menu*);
};

#endif /* STATES_OPTIONSMENUSTATE_H_ */
