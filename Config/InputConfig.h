//
// Created by axel on 9/8/18.
//

#ifndef PANEL_POP_INPUTCONFIG_H
#define PANEL_POP_INPUTCONFIG_H

#include "../InputState.h"
#include "../InputEvents/InputEvent.h"

class InputConfig {
 public:
  InputConfig(const InputEvent &up,
              const InputEvent &down,
              const InputEvent &left,
              const InputEvent &right,
              const InputEvent &swap,
              const InputEvent &raiseStack);

  const InputEvent &_up;
  const InputEvent &_down;
  const InputEvent &_left;
  const InputEvent &_right;

  const InputEvent &_swap;
  const InputEvent &_raiseStack;
};

#endif //PANEL_POP_INPUTCONFIG_H
