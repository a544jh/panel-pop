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
				_boardYPos(y),
				_SDLContext(SDLContext::getInstance()),
				_lastFrameCursorMove(false),
				_thisFrameCursorMove(false),
				_combo(false),
				_chain(false),
				_blockFall(false) {

}

BoardEventHandler::~BoardEventHandler() {
}

void BoardEventHandler::endTick() {
	_lastFrameCursorMove = _thisFrameCursorMove;
	_thisFrameCursorMove = false;

	//chain sound overrides combo...
	if (_chain) {
		Mix_PlayChannel(-1, _SDLContext._sfxChain, 0);
		_chain = false;
	} else if (_combo) {
		Mix_PlayChannel(-1, _SDLContext._sfxCombo, 0);
		_combo = false;
	}


	//prevent multiple
	if (_blockFall) {
		Mix_PlayChannel(-1, _SDLContext._sfxThump, 0);
		_blockFall = false;
	}

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

	Mix_PlayChannel(-1, _SDLContext._sfxPop[soundOrder + 10 * (soundChain - 1)],
			0);
}

void BoardEventHandler::cursorMove() {
	_thisFrameCursorMove = true;
	if (!_lastFrameCursorMove) {
		Mix_PlayChannel(-1, _SDLContext._sfxCursor, 0);
	}
}

void BoardEventHandler::swap() {
	Mix_PlayChannel(-1, _SDLContext._sfxSwap, 0);
}

void BoardEventHandler::blockFall() {
	_blockFall = true;

}

void BoardEventHandler::gbFall() {
	Mix_PlayChannel(-1, _SDLContext._sfxBigThump, 0);
}

void BoardEventHandler::combo() {
	_combo = true;
}

void BoardEventHandler::chain() {
	_chain = true;
}

void BoardEventHandler::chainEnd(int chain) {
	if (chain == 4) {
		Mix_PlayChannel(-1, _SDLContext._sfxFanfare1, 0);
	} else if (chain == 5) {
		Mix_PlayChannel(-1, _SDLContext._sfxFanfare2, 0);
	} else if (chain >= 6) {
		Mix_PlayChannel(-1, _SDLContext._sfxFanfare3, 0);
	}
}
