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
	GameState();
	void tick();
	SDL_Texture* render();
	virtual ~GameState();
private:
	Game game;
	GameRenderer renderer;
	KeyboardController kbc;
};

#endif /* STATES_GAMESTATE_H_ */
