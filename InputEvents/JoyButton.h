//
// Created by axel on 4/21/19.
//

#ifndef PANEL_POP_INPUTEVENTS_JOYBUTTON_H_
#define PANEL_POP_INPUTEVENTS_JOYBUTTON_H_

#include "InputEvent.h"
class JoyButton: public InputEvent {
 public:
  JoyButton(SDL_JoystickID joystick_id, int button_id);
  std::string toString() override;
  bool isActive() const override;

 private:
  SDL_JoystickID _joystickId;
  int _buttonId;
  SDL_Joystick *_joystick;

};

#endif //PANEL_POP_INPUTEVENTS_JOYBUTTON_H_
