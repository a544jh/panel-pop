/*
 * Renderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../SDLContext.h"
#include <SDL2/SDL.h>

class Renderer {
public:
	Renderer();
	virtual ~Renderer();
protected:
	SDLContext& _SDLContext;
	SDL_Renderer* _SDLRenderer;
	SDL_Texture* _texture;
};

#endif /* RENDERER_H_ */
