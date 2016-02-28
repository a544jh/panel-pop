/*
 * GameRenderer.h
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#ifndef GAMERENDERER_H_
#define GAMERENDERER_H_

#include <list>

#include "../Rendering/Particle.h"

class GameRenderer: public Renderer {
public:


	static const int BOARD0_X = 58;
	static const int BOARD0_Y = 43;
	static const int BOARD1_X = 390;
	static const int BOARD1_Y = 43;

	GameRenderer();
	virtual ~GameRenderer();

	virtual void tick() =0;
	virtual SDL_Texture* renderGame() =0;

	void addParticle(Particle*);

	virtual void shakeBoard(int id, int duration) =0;
protected:
	std::list<Particle*> _particles;
	void handleParticles();
	void renderParticles();
};

#endif /* GAMERENDERER_H_ */
