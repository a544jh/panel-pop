/*
 * State.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATE_H_
#define STATE_H_

#include "SDL2/SDL.h"

class State {
 public:
  State();
  virtual ~State();
  virtual SDL_Texture *render() = 0;
  virtual void tick() = 0;
  virtual void goBack() = 0;
};

#endif /* STATE_H_ */
