
/* 
 * File:   FadeTransition.h
 * Author: axel
 *
 * Created on October 9, 2016, 10:33 PM
 */

#ifndef FADETRANSITION_H
#define FADETRANSITION_H

#include "State.h"
#include <functional>

class FadeTransition : public State {
  typedef std::function<State *()> StateConstructor;

 public:
  FadeTransition(StateConstructor);
  const int TRANSITION_TICKS = 120;

  SDL_Texture *render() override;
  void tick() override;
  void goBack() override;

  virtual ~FadeTransition();
 private:
  StateConstructor _nextStateConstr;
  int _ticksRun;
  State *_nextState;
};

#endif /* FADETRANSITION_H */

