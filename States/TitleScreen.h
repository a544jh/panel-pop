/*
 * TitleScreen.h
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#ifndef STATES_TITLESCREEN_H_
#define STATES_TITLESCREEN_H_

#include "../Rendering/TitleScreenRenderer.h"
#include "State.h"

class TitleScreen: public State {
public:
	TitleScreen();
	virtual ~TitleScreen();
	void tick();
	SDL_Texture* render();
private:
	TitleScreenRenderer _tr;
};


#endif /* STATES_TITLESCREEN_H_ */
