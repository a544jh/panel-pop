//
// Created by axel on 9/8/18.
//

#ifndef PANEL_POP_INPUTSTATE_H
#define PANEL_POP_INPUTSTATE_H


enum Direction {
    UP, RIGHT, DOWN, LEFT, NONE
};

class InputState {
public:
    Direction _direction;
    bool _swap;
    bool _raiseStack;

    InputState(Direction direction, bool swap, bool raiseStack);

    static InputState getCurrentState(); //TODO add config parameter...
};


#endif //PANEL_POP_INPUTSTATE_H
