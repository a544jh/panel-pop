/*
 * GameState.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATES_VSGAMESTATE_H_
#define STATES_VSGAMESTATE_H_

#include "../Game/KeyboardController.h"
#include "../Game/VsGame.h"
#include "../Rendering/VsGameRenderer.h"
#include "State.h"

class VsGameState : public State {
public:
	VsGameState();
	void tick();
	SDL_Texture* render();
	virtual ~VsGameState();
	void goBack();
private:
	VsGame _vsGame;
	VsGameRenderer _renderer;
	KeyConfig _p1keys;
	KeyConfig _p2keys;
	KeyboardController _kbc;
	KeyboardController _kbc2;
};

#endif /* STATES_VSGAMESTATE_H_ */
