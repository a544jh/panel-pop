/*
 * SDLContext.h
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#ifndef SDLCONTEXT_H_
#define SDLCONTEXT_H_

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

	TTF_Font* _psFont;
	TTF_Font* _squareFont;
private:
	SDLContext();
	SDLContext(SDLContext const&) = delete;
	void operator=(SDLContext const&) = delete;

	SDL_Window* _window;
	SDL_Renderer* _renderer;
	SDL_Texture* _spriteSheet;

	bool loadSpriteSheet();
	bool loadFonts();
};

#endif /* SDLCONTEXT_H_ */
