/*
 * TitleScreen.h
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#ifndef STATES_TITLESCREEN_H_
#define STATES_TITLESCREEN_H_

#include "../Menus/MainMenu.h"
#include "../Rendering/TitleScreenRenderer.h"
#include "State.h"

class TitleScreen: public State {
public:
	TitleScreen();
	virtual ~TitleScreen();
	void tick();
	SDL_Texture* render();
	const MainMenu& getMenu() const;

private:
	TitleScreenRenderer _tr;
	MainMenu _menu;
};


#endif /* STATES_TITLESCREEN_H_ */
