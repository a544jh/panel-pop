/*
 * GameState.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATES_GAMESTATE_H_
#define STATES_GAMESTATE_H_

#include "State.h"
#include "../Game/Game.h"
#include "../Rendering/GameRenderer.h"

class GameState : public State {
public:
	GameState(KeyboardControllerConfig&, KeyboardControllerConfig&);
	void tick();
	SDL_Texture* render();
	virtual ~GameState();
private:
	Game _game;
	GameRenderer _renderer;
	KeyboardController _kbc;
	KeyboardController _kbc2;
};

#endif /* STATES_GAMESTATE_H_ */
