/*
 * TitleScreenRenderer.cpp
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#include "TitleScreenRenderer.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <string>

#include "../Main.h"
#include "../SDLContext.h"
#include "../States/TitleScreen.h"

TitleScreenRenderer::TitleScreenRenderer(TitleScreen &ts) :
    _titleScreen(ts) {
    _texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 640, 480);
    _titleImg = _SDLContext.makeTextureFromImage("assets/title.png");
    _versionText = _SDLContext.makeTextureFromFont(PANELPOP_VERSION,
                                                   {0, 0, 0}, _SDLContext._fontPs);
}

TitleScreenRenderer::~TitleScreenRenderer() {
    SDL_DestroyTexture(_titleImg);
    SDL_DestroyTexture(_versionText);
}

SDL_Texture *TitleScreenRenderer::render() {
    SDL_SetRenderTarget(_SDLRenderer, _texture);
    SDL_RenderCopy(_SDLRenderer, _titleImg, NULL, NULL);
    SDL_Rect versionpos = {159, 464, 0, 0};
    SDL_QueryTexture(_versionText, NULL, NULL, &versionpos.w, &versionpos.h);
    SDL_RenderCopy(_SDLRenderer, _versionText, NULL, &versionpos);
    //blinking text
    if (SDL_GetTicks() % 1000 < 500) {
        SDL_SetRenderDrawColor(_SDLRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_Rect box = {311, 262, 190, 14};
        SDL_RenderFillRect(_SDLRenderer, &box);
    }

    _titleScreen.getMenu().render();

//	SDL_Color color = {0,0,0};
//	SDL_Texture* testfont = _SDLContext.makeTextureFromFont("PANEL POP",color,_SDLContext._squareFont);
//	SDL_SetTextureAlphaMod(testfont, 0x50);
//	SDL_Rect testr {50,350,0,0};
//	SDL_QueryTexture(testfont,NULL,NULL,&testr.w,&testr.h);
//	SDL_RenderCopy(_SDLRenderer, testfont,NULL,&testr);
//	SDL_DestroyTexture(testfont);
    return _texture;
}
