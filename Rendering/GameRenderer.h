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

class VsGame;

class GameRenderer: public Renderer {
public:
	GameRenderer(VsGame&);
	virtual ~GameRenderer();
	void tick();
	SDL_Texture* renderGame();

	static const int BOARD0_X = 58;
	static const int BOARD0_Y = 43;
	static const int BOARD1_X = 390;
	static const int BOARD1_Y = 43;

	void addPopup(Popup*);
	void addParticle(Particle*);

	void shakeBoard(int id, int duration);

private:
	VsGame& _game;
	BoardRenderer _boardRenderer0;
	BoardRenderer _boardRenderer1;
	GbQueueRenderer _gbqr0;
	GbQueueRenderer _gbqr1;
	SDL_Texture* _bg;
	SDL_Texture* _2pbg;

	int _b0Shake;
	int _b1Shake;

	std::list<Particle*> _particles;

	void renderBoard(int id);
	void renderStatsText();
	void renderMatchPoints();
	void renderPopups();
	void handleParticles();
	void renderParticles();
	void handleShake();
};

#endif /* GAMERENDERER_H_ */
