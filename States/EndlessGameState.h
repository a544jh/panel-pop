/*
 * EndlessGameState.h
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#ifndef STATES_ENDLESSGAMESTATE_H_
#define STATES_ENDLESSGAMESTATE_H_

#include "../Game/EndlessGame.h"
#include "../Game/KeyboardController.h"
#include "../Rendering/EndlessGameRenderer.h"
#include "State.h"

class EndlessGameState: public State {
public:
	EndlessGameState();
	virtual ~EndlessGameState();

	void tick();
	SDL_Texture* render();
	void goBack();

private:
	EndlessGame _endlessGame;
	EndlessGameRenderer _renderer;
	KeyboardController _kbc;

};

#endif /* STATES_ENDLESSGAMESTATE_H_ */
