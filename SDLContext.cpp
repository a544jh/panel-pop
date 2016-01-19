/*
 * SDLContext.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include "SDLContext.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

SDLContext::SDLContext() :
_window(nullptr),
_renderer(nullptr),
_spriteSheet(nullptr),
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

	_window = SDL_CreateWindow("Panel pop",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

	int imgFlags = IMG_INIT_PNG;
	if(! (IMG_Init(imgFlags) & imgFlags)){
		std::cout << IMG_GetError();
		success = false;
	}


	success = loadSpriteSheet();
	return success;
}

SDL_Renderer* SDLContext::getRenderer() {
	return _renderer;
}

SDL_Texture* SDLContext::getSpriteSheet(){
	return _spriteSheet;
}

bool SDLContext::loadSpriteSheet() {
	std::string path = "assets/sprites.png";
	SDL_Surface* surface = IMG_Load(path.c_str());
	if(surface == NULL){
		std::cout << IMG_GetError();
		return false;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_FreeSurface(surface);
	_spriteSheet = texture;
	return true;
}

void SDLContext::renderTextureToWindow(SDL_Texture* texture)
{
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_RenderCopy(_renderer, texture, NULL, NULL);
	SDL_RenderPresent(_renderer);
}

void SDLContext::toggleFullscreen() {
	uint32_t flags = SDL_GetWindowFlags(_window);
	if ((flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN){
		SDL_SetWindowFullscreen(_window,0);
		SDL_ShowCursor(SDL_ENABLE);
	} else {
		SDL_SetWindowFullscreen(_window,SDL_WINDOW_FULLSCREEN);
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void SDLContext::tearDown(){
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}
