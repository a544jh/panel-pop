/*
 * SDLContext.h
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#ifndef SDLCONTEXT_H_
#define SDLCONTEXT_H_

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class SDLContext {
public:
	const int WINDOW_WIDTH, WINDOW_HEIGHT;

	static SDLContext& getInstance();
	bool init();
	void tearDown();
	SDL_Renderer* getRenderer();
	SDL_Texture* getSpriteSheet();
	void renderTextureToWindow(SDL_Texture*);
	void toggleFullscreen();

	SDL_Texture* makeTextureFromImage(std::string);
	SDL_Texture* makeTextureFromFont(std::string, SDL_Color, TTF_Font*);
	void renderText(std::string, SDL_Color, TTF_Font*, int, int);

	TTF_Font* _fontPs;
	TTF_Font* _fontSquare;

	Mix_Music* _musicBg;
	Mix_Chunk* _sfxPop[40];
	Mix_Chunk* _sfxCursor;
	Mix_Chunk* _sfxSwap;
	Mix_Chunk* _sfxThump;
	Mix_Chunk* _sfxBigThump;
	Mix_Chunk* _sfxCombo;
	Mix_Chunk* _sfxChain;
	Mix_Chunk* _sfxFanfare1;
	Mix_Chunk* _sfxFanfare2;
	Mix_Chunk* _sfxFanfare3;


private:
	SDLContext();
	SDLContext(SDLContext const&) = delete;
	void operator=(SDLContext const&) = delete;

	SDL_Window* _window;
	SDL_Renderer* _renderer;
	SDL_Texture* _spriteSheet;

	bool loadSpriteSheet();
	bool loadFonts();
	bool loadAudio();
};

#endif /* SDLCONTEXT_H_ */
