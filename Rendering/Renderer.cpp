/*
 * Renderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Renderer.h"

Renderer::Renderer() :
		_texture(nullptr),
		_SDLContext(SDLContext::getInstance()),
		_SDLRenderer(_SDLContext.getRenderer()),
		_spriteSheet(_SDLContext.getSpriteSheet())
{}

Renderer::~Renderer() {
	SDL_DestroyTexture(_texture);
}

