/*
 * GameRenderer.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#include "GameRenderer.h"

#include <SDL2/SDL_pixels.h>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>

#include "../Game/Board.h"
#include "../SDLContext.h"

class Particle;
class Popup;

GameRenderer::GameRenderer(Game &game) :
    _game(game) {
}

GameRenderer::~GameRenderer() {
}

void GameRenderer::addParticle(Particle *p) {
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

void GameRenderer::renderGameTime(int x, int y) {
    std::ostringstream os;
    uint32_t time;
    if (_game.getBoard(0).getState() == Board::COUNTDOWN) {
        time = 0;
    } else {
        time = _game.getTime() - 3000; //TODO: extract constant
    }
    os << (time / 1000 / 60) << "\u2019" << std::setw(2) << std::setfill('0')
       << ((time / 1000) % 60) << "\u201d";
    _SDLContext.renderText(os.str(), {255, 255, 255}, _SDLContext._fontPs, x,
                           y);
}
