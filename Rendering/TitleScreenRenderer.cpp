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
	_titleImg = _SDLContext.makeTextureFromImage("assets/title.png");
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

//	SDL_Color color = {0,0,0};
//	SDL_Texture* testfont = _SDLContext.makeTextureFromFont("PANEL POP",color,_SDLContext._squareFont);
//	SDL_SetTextureAlphaMod(testfont, 0x50);
//	SDL_Rect testr {50,350,0,0};
//	SDL_QueryTexture(testfont,NULL,NULL,&testr.w,&testr.h);
//	SDL_RenderCopy(_SDLRenderer, testfont,NULL,&testr);
//	SDL_DestroyTexture(testfont);
	return _texture;
}
