/*
 * SDLContext.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include "SDLContext.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <iostream>
#include <sstream>

SDLContext::SDLContext() :
				WINDOW_WIDTH(640),
				WINDOW_HEIGHT(480),
				_psFont(nullptr),
				_squareFont(nullptr),
				_bgSong(nullptr),
				_window(nullptr),
				_renderer(nullptr),
				_spriteSheet(nullptr) {
}

SDLContext& SDLContext::getInstance() {
	static SDLContext instance;
	return instance;
}

bool SDLContext::init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << SDL_GetError();
		success = false;
	}

	_window = SDL_CreateWindow("Panel pop", SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (_window == NULL) {
		std::cout << SDL_GetError();
		success = false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	if (_renderer == NULL) {
		std::cout << SDL_GetError();
		success = false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cout << IMG_GetError();
		success = false;
	}

	if (TTF_Init() == -1) {
		std::cout << TTF_GetError();
		success = false;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << Mix_GetError();
		success = false;
	}

	Mix_VolumeMusic(MIX_MAX_VOLUME);

	success = loadSpriteSheet();
	success = loadFonts();
	success = loadAudio();

	//Mix_PlayMusic(_bgSong, -1);

	return success;
}

SDL_Renderer* SDLContext::getRenderer() {
	return _renderer;
}

SDL_Texture* SDLContext::getSpriteSheet() {
	return _spriteSheet;
}

bool SDLContext::loadSpriteSheet() {
	_spriteSheet = makeTextureFromImage("assets/sprites.png");
	return _spriteSheet != NULL;
}

bool SDLContext::loadFonts() {
	_psFont = TTF_OpenFont("assets/fonts/PressStart2P.ttf", 16);
	_squareFont = TTF_OpenFont("assets/fonts/square_sans_serif_7.ttf", 48);
	if (_psFont == NULL || _squareFont == NULL) {
		std::cout << TTF_GetError();
		return false;
	}
	return true;
}

SDL_Texture* SDLContext::makeTextureFromFont(std::string text, SDL_Color color,
		TTF_Font* font) {
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (surface == NULL) {
		std::cout << TTF_GetError();
		return NULL;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* SDLContext::makeTextureFromImage(std::string path) {
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		std::cout << IMG_GetError();
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void SDLContext::renderText(std::string text, SDL_Color color, TTF_Font* font,
		int x, int y) {
	SDL_Texture* texture = makeTextureFromFont(text, color, font);
	SDL_Rect r = { x, y, 0, 0 };
	SDL_QueryTexture(texture, NULL, NULL, &r.w, &r.h);
	SDL_RenderCopy(_renderer, texture, NULL, &r);
	SDL_DestroyTexture(texture);
}

void SDLContext::renderTextureToWindow(SDL_Texture* texture) {
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_RenderCopy(_renderer, texture, NULL, NULL);
	SDL_RenderPresent(_renderer);
}

void SDLContext::toggleFullscreen() {
	uint32_t flags = SDL_GetWindowFlags(_window);
	if ((flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN) {
		SDL_SetWindowFullscreen(_window, 0);
		SDL_ShowCursor(SDL_ENABLE);
	} else {
		SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void SDLContext::tearDown() {
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

bool SDLContext::loadAudio() {
	_bgSong = Mix_LoadMUS("assets/music/battle1_loop.ogg");
	std::cout << Mix_GetError();
	//_popSfx = Mix_LoadWAV("assets/sfx/1x1.wav");

	for (int i = 0; i < 40; ++i) {
		std::ostringstream os;
		os << "assets/sfx/" << (i / 10) + 1 << "x" << ((i % 10) + 1) << ".wav";
		std::cout << os.str();
		_popSfx[i] = Mix_LoadWAV(os.str().c_str());
	}

	std::cout << Mix_GetError();
	return _bgSong != NULL;
}
