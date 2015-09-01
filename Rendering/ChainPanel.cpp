/*
 * ChainPanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "ChainPanel.h"

ChainPanel::ChainPanel() {
	// TODO Auto-generated constructor stub

}

ChainPanel::ChainPanel(int x, int y, int value) {
	_x = x;
	_y = y;
	_value = value;
	_h = 30;
	_w = 35;
	_ticksAlive = 60;
	_texture = createTexture();
}

ChainPanel::~ChainPanel() {
	SDL_DestroyTexture(_texture);
}

SDL_Texture* ChainPanel::createTexture() {
	SDL_Texture* texture;
	texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, _w, _h);
	SDL_SetRenderTarget(_SDLRenderer, texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0xE0, 0x00, 0xFF);
	SDL_RenderClear(_SDLRenderer);
	return texture;
}
