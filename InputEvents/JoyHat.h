//
// Created by axel on 4/22/19.
//

#ifndef PANEL_POP_INPUTEVENTS_JOYHAT_H_
#define PANEL_POP_INPUTEVENTS_JOYHAT_H_

#include "InputEvent.h"
#import "SDL.h"
class JoyHat: public InputEvent {
 public:
  JoyHat(SDL_JoystickID joystick_id, int hat_id, Uint8 hat_direction);
  std::string toString() override;
  bool isActive() const override;

 private:
  SDL_JoystickID _joystickId;
  int _hatId;
  Uint8 _hatDirection;
  SDL_Joystick *_joystick;
};

#endif //PANEL_POP_INPUTEVENTS_JOYHAT_H_
