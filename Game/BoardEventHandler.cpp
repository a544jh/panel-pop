/*
 * BoardEventHandler.cpp
 *
 *  Created on: 7.2.2016
 *      Author: axelw
 */

#include "BoardEventHandler.h"

#include <SDL2/SDL_mixer.h>
#include <iostream>

#include "../Rendering/BoardRenderer.h"
#include "../Rendering/ChainPopup.h"
#include "../Rendering/ComboPopup.h"
#include "../SDLContext.h"

BoardEventHandler::BoardEventHandler(GameRenderer& gr, int x, int y) :
				_gr(gr),
				_boardXPos(x),
				_boardYPos(y) {

}

BoardEventHandler::~BoardEventHandler() {
}

void BoardEventHandler::blockExplode(int x, int y, int stackOffset, int order,
		int chain) {
	int posx = x * BoardRenderer::TILE_SIZE + 5 + _boardXPos;
	int posy = BoardRenderer::BOARD_HEIGHT - (y + 1) * BoardRenderer::TILE_SIZE
			- stackOffset + _boardYPos;
	_gr.addPopup(new ComboPopup(posx, posy, order, 100)); // TODO: change to particle
	_gr.addPopup(new ChainPopup(posx + 20, posy, chain, 100));

	int soundOrder = order;
	if (soundOrder > 9) {
		soundOrder = 9;
	}
	int soundChain = chain;
	if (soundChain > 4) {
		soundChain = 4;
	}

	Mix_PlayChannel(1,
			SDLContext::getInstance()._popSfx[soundOrder + 10 * (soundChain - 1)],
			0);
}
