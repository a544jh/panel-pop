
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
#include <memory>

class FadeTransition : public State {
  typedef std::function<std::unique_ptr<State>()> StateConstructor;

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
  std::unique_ptr<State> _nextState;
  SDL_Texture *_lastTarget; //kept so render() still works after the state was handed off to StateManager
};

#endif /* FADETRANSITION_H */

