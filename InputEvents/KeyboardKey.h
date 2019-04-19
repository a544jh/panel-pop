//
// Created by axel on 4/19/19.
//

#ifndef PANEL_POP_KEYBOARDKEY_H
#define PANEL_POP_KEYBOARDKEY_H

#include "InputEvent.h"

class KeyboardKey : public InputEvent {
 public:
  KeyboardKey(int scancode);

  bool isActive() const override;

 private:
  std::string toString() override;

 private:
  int _scancode;
};

#endif //PANEL_POP_KEYBOARDKEY_H
