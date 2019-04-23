//
// Created by axel on 9/8/18.
//

#ifndef PANEL_POP_INPUTSTATE_H
#define PANEL_POP_INPUTSTATE_H

class InputConfig;

enum Direction {
  UP, RIGHT, DOWN, LEFT, NONE
};

struct InputState {
 public:
  Direction _direction;
  bool _swap;
  bool _raiseStack;
  bool _start;

  InputState();

  InputState(Direction direction, bool swap, bool raise_stack, bool start);

  static InputState getCurrentState(InputConfig &);
 private:
  static Direction getDirection(InputConfig &);
};

#endif //PANEL_POP_INPUTSTATE_H
