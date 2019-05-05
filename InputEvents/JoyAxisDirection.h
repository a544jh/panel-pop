//
// Created by axel on 4/22/19.
//

#ifndef PANEL_POP_INPUTEVENTS_JOYAXISDIRECTION_H_
#define PANEL_POP_INPUTEVENTS_JOYAXISDIRECTION_H_

#include <SDL_joystick.h>
#include "InputEvent.h"
class JoyAxisDirection: public InputEvent {
 public:

  enum Direction {
    NEGATIVE, POSITIVE
  };

  JoyAxisDirection(SDL_JoystickID joystick_id, int axis_id, Direction direction);
  std::string toString() override;
  bool isActive() const override;
  static const Sint16 AXIS_THRESHOLD = 16383;

 private:
  SDL_JoystickID _joystickId;
  int _axisId;
  Direction _direction;
  SDL_Joystick *_joystick;

};

#endif //PANEL_POP_INPUTEVENTS_JOYAXISDIRECTION_H_
