//
// Created by axel on 4/19/19.
//

#ifndef PANEL_POP_INPUTEVENT_H
#define PANEL_POP_INPUTEVENT_H

#include <string>

class InputEvent {

 public:
  virtual std::string toString() = 0;
  static InputEvent &fromString(std::string str);

  virtual bool isActive() const = 0;

};

#endif //PANEL_POP_INPUTEVENT_H
