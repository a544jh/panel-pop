/*
 * GameRenderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAMERENDERER_H_
#define GAMERENDERER_H_

#include <list>

#include "BoardRenderer.h"
#include "GbQueueRenderer.h"
#include "Particle.h"
#include "Popup.h"

class GameRenderer: public Renderer {
public:
	GameRenderer(Game&);
	virtual ~GameRenderer();
	void tick();
	SDL_Texture* renderGame();

	static const int BOARD1_X = 58;
	static const int BOARD1_Y = 43;
	static const int BOARD2_X = 390;
	static const int BOARD2_Y = 43;

	void addPopup(Popup*);
	void addParticle(Particle*);

private:
	Game& _game;
	BoardRenderer _boardRenderer;
	BoardRenderer _boardRenderer2;
	GbQueueRenderer _gbqr;
	GbQueueRenderer _gbqr2;
	SDL_Texture* _bg;
	SDL_Texture* _2pbg;

	std::list<Particle*> _particles;

	void renderStatsText();
	void renderMatchPoints();
	void renderPopups();
	void handleParticles();
	void renderParticles();
};

#endif /* GAMERENDERER_H_ */
