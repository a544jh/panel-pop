/*
 * GameRenderer.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#include "GameRenderer.h"

class Particle;
class Popup;

GameRenderer::GameRenderer() {

}

GameRenderer::~GameRenderer() {
}


void GameRenderer::addParticle(Particle* p) {
	_particles.push_back(p);
}

void GameRenderer::handleParticles() {
	auto it = _particles.begin();
		while (it != _particles.end()) {
			if ((*it)->_alive) {
				(*it)->tick();
				++it;
			} else {
				delete *it;
				it = _particles.erase(it);
			}
		}
}

void GameRenderer::renderParticles() {
	for (auto it = _particles.begin(); it != _particles.end(); ++it) {
			(*it)->render();
		}
}
