/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "GameRenderer.h"

GameRenderer::GameRenderer(Game& game) :
_game(game),
_boardRenderer(_game._board)
{
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);

}

void GameRenderer::tick() {
	_boardRenderer.tick();
}

SDL_Texture* GameRenderer::renderGame() {

	SDL_Texture* boardTexture = _boardRenderer.renderBoard();

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	SDL_Rect rekt; rekt.x = 0; rekt.y = 0;
	rekt.w = _boardRenderer.BOARD_WIDTH;
	rekt.h = _boardRenderer.BOARD_HEIGHT;
	SDL_RenderCopy(_SDLRenderer,boardTexture, NULL, &rekt);

	return _texture;
}

GameRenderer::~GameRenderer() {
}

