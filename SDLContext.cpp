/*
 * SDLContext.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include "SDLContext.h"
#include <SDL2/SDL.h>
#include <iostream>

SDLContext::SDLContext() :
_window(nullptr),
_renderer(nullptr),
WINDOW_WIDTH(640),
WINDOW_HEIGHT(480)
{}

SDLContext& SDLContext::getInstance()
{
	static SDLContext instance;
	return instance;
}

bool SDLContext::init()
{
	bool success = true;

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << SDL_GetError();
		success = false;
	}

	_window = SDL_CreateWindow("Panel pop",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (_window == NULL)
	{
		std::cout << SDL_GetError();
		success = false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	if (_renderer == NULL)
		{
			std::cout << SDL_GetError();
			success = false;
		}

	return success;
}

SDL_Renderer* SDLContext::getRenderer() {
	return _renderer;
}

void SDLContext::renderTextureToWindow(SDL_Texture* texture)
{
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_RenderCopy(_renderer, texture, NULL, NULL);
	SDL_RenderPresent(_renderer);
}

void SDLContext::tearDown(){
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}
