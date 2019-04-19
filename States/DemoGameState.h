
/* 
 * File:   DemoGameState.h
 * Author: axel
 *
 * Created on October 5, 2016, 9:36 PM
 */

#ifndef DEMOGAMESTATE_H
#define DEMOGAMESTATE_H

#include "GameState.h"

class DemoGameState : public GameState {
 public:
  DemoGameState();

  void tick() override;
  SDL_Texture *render() override;

  virtual ~DemoGameState();
 private:

};

#endif /* DEMOGAMESTATE_H */

