/*
 * Renderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */


#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL2/SDL.h>
#include "../SDLContext.h"


class Renderer {
public:
	Renderer();
	virtual ~Renderer();
	//Inherit to get access to the SDL renderer.
	SDLContext& _SDLContext;
	SDL_Texture* _texture;
	SDL_Renderer* _SDLRenderer;
	SDL_Texture* _spriteSheet;
};

#endif /* RENDERER_H_ */
