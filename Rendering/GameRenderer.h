/*
 * GameRenderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAMERENDERER_H_
#define GAMERENDERER_H_

#include "../Game/Game.h"
#include "Renderer.h"
#include "BoardRenderer.h"
#include <SDL2/SDL.h>

#include "GbQueueRenderer.h"
class GameRenderer: public Renderer {
public:
	GameRenderer(Game&);
	virtual ~GameRenderer();
	void tick();
	SDL_Texture* renderGame();
private:
	Game& _game;
	BoardRenderer _boardRenderer;
	BoardRenderer _boardRenderer2;
	GbQueueRenderer _gbqr;
	GbQueueRenderer _gbqr2;
	SDL_Texture* _bg;
	SDL_Texture* _2pbg;
	void renderStatsText();
};

#endif /* GAMERENDERER_H_ */
