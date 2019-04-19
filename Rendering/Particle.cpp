/*
 * Particle.cpp
 *
 *  Created on: 8.2.2016
 *      Author: axelw
 */

#include "Particle.h"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

Particle::Particle() :
    _x(0),
    _y(0),
    _vx(0),
    _vy(0),
    _ticksToLive(0),
    _ticksLived(0),
    _alive(true) {
}

Particle::Particle(int x, int y, int vx, int vy, int lifetime) :
    _x(x),
    _y(y),
    _vx(vx),
    _vy(vy),
    _ticksToLive(lifetime),
    _ticksLived(0),
    _alive(true) {
}

void Particle::tick() {
    _x += _vx;
    _y += _vy;
    if (++_ticksLived >= _ticksToLive) {
        _alive = false;
    }
}

void Particle::render() {
    SDL_Rect sprite = {0, 382, 5, 5};
    SDL_Rect pos = {_x, _y, 5, 5};
    SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
}

Particle::~Particle() {
}

