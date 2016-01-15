/*
 * TitleScreenRenderer.cpp
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#include "TitleScreenRenderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

TitleScreenRenderer::TitleScreenRenderer() {
	std::string path = "assets/title.png";
	SDL_Surface* surface = IMG_Load(path.c_str());
	_titleImg = SDL_CreateTextureFromSurface(_SDLRenderer, surface);
	SDL_FreeSurface(surface);
}

TitleScreenRenderer::~TitleScreenRenderer() {
	SDL_DestroyTexture(_titleImg);
}

SDL_Texture* TitleScreenRenderer::render() {
	SDL_RenderCopy(_SDLRenderer,_titleImg,NULL,NULL);
	//blinking text
	if(SDL_GetTicks() % 1000 < 500){
		SDL_SetRenderDrawColor(_SDLRenderer,0xff,0xff,0xff,0xff);
		SDL_Rect box = {311,262,190,14};
		SDL_RenderFillRect(_SDLRenderer,&box);
	}
	return _texture;
}
