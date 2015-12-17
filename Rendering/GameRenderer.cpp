/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "GameRenderer.h"

GameRenderer::GameRenderer(Game& game) :
		_game(game), _boardRenderer(_game._board), _boardRenderer2(_game._board2) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);

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

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	SDL_Rect rekt;
	rekt.x = 0;
	rekt.y = 0;
	rekt.w = _boardRenderer.BOARD_WIDTH;
	rekt.h = _boardRenderer.BOARD_HEIGHT;
	SDL_RenderCopy(_SDLRenderer, boardTexture, NULL, &rekt);

	rekt.x = 300;

	SDL_RenderCopy(_SDLRenderer, boardTexture2, NULL, &rekt);

	if (_game.isPaused()) {
		SDL_SetTextureColorMod(_texture, 0x50, 0x50, 0x50);
	} else {
		SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
	}

	return _texture;
}

GameRenderer::~GameRenderer() {
}

