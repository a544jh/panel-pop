//

//
// Created by axel on 9/8/18.
#ifndef PANEL_POP_INPUTCONFIG_H
#define PANEL_POP_INPUTCONFIG_H

#include <memory>
#include "../InputState.h"
#include "../InputEvents/InputEvent.h"

class InputConfig {
public:
    InputConfig(InputEvent *up,
                InputEvent *down,
                InputEvent *left,
                InputEvent *right,
                InputEvent *swap,
                InputEvent *raiseStack,
                InputEvent *start);

    std::shared_ptr<InputEvent> _up;
    std::shared_ptr<InputEvent> _down;
    std::shared_ptr<InputEvent> _left;
    std::shared_ptr<InputEvent> _right;

    std::shared_ptr<InputEvent> _swap;
    std::shared_ptr<InputEvent> _raiseStack;
    std::shared_ptr<InputEvent> _start;

    void updateState();

    Direction getDownDirection() const;// get the direction input that started on this tick, otherwise NONE
    bool raiseStackDown() const;

    bool swapDown() const;
    bool startDown() const;

    InputState _currentState;
    InputState _prevState;
};

#endif //PANEL_POP_INPUTCONFIG_H
