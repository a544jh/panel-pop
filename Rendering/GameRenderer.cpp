/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "GameRenderer.h"

GameRenderer::GameRenderer(Game& game) :
		_game(game), _boardRenderer(_game._board), _boardRenderer2(
				_game._board2), _gbqr(_game._board), _gbqr2(_game._board2) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);
	_bg = _SDLContext.makeTextureFromImage("assets/bg1.png");
	_2pbg = _SDLContext.makeTextureFromImage("assets/2p.png");
	SDL_SetTextureBlendMode(_2pbg, SDL_BLENDMODE_BLEND);
}

void GameRenderer::tick() {
	if (!_game.isPaused()) {
		_boardRenderer.tick();
		_boardRenderer2.tick();
	}
}

SDL_Texture* GameRenderer::renderGame() {

	SDL_Texture* boardTexture = _boardRenderer.renderBoard();
	SDL_Texture* boardTexture2 = _boardRenderer2.renderBoard();
	SDL_Texture* gbq = _gbqr.renderQueue();
	SDL_Texture* gbq2 = _gbqr2.renderQueue();

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	SDL_RenderCopy(_SDLRenderer, _bg, NULL, NULL);
	SDL_RenderCopy(_SDLRenderer, _2pbg, NULL, NULL);

	SDL_Rect rekt;
	rekt.x = 58;
	rekt.y = 43;
	rekt.w = _boardRenderer.BOARD_WIDTH;
	rekt.h = _boardRenderer.BOARD_HEIGHT;
	SDL_RenderCopy(_SDLRenderer, boardTexture, NULL, &rekt);
	SDL_Rect gbqp = { 258, 307, 38, 120 };
	SDL_RenderCopy(_SDLRenderer, gbq, NULL, &gbqp);

	rekt.x = 390;
	gbqp.x = 344;

	SDL_RenderCopy(_SDLRenderer, boardTexture2, NULL, &rekt);
	SDL_RenderCopy(_SDLRenderer, gbq2, NULL, &gbqp);

	if (_game.isPaused()) {
		SDL_SetTextureColorMod(_texture, 0x50, 0x50, 0x50);
	} else {
		SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
	}

	return _texture;
}

GameRenderer::~GameRenderer() {
	SDL_DestroyTexture(_bg);
	SDL_DestroyTexture(_2pbg);
}

