/*
 * Particle.h
 *
 *  Created on: 8.2.2016
 *      Author: axelw
 */

#ifndef RENDERING_PARTICLE_H_
#define RENDERING_PARTICLE_H_

#include <string>

#include "Renderer.h"

class Particle : public Renderer {
 public:
  Particle();
  Particle(int x, int y, int vx, int vy, int lifetime);

  int _x, _y, _vx, _vy, _ticksToLive, _ticksLived;
  bool _alive;

  virtual void tick();
  virtual void render();
  virtual ~Particle();
};

#endif /* RENDERING_PARTICLE_H_ */
