/*
 * StateManager.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include <cstdint>

#include "../Config/KeyConfig.h"

class InputManager;
class SDLContext;
class State;

class StateManager {
public:
    static StateManager& getInstance();
    void run();
    void switchToState(State*);
    void returnToTitle();
    float getAvgFps() const;
    State* getCurrentState() const;
    const KeyConfig& getP1keys() const;
    void setKeys(KeyConfig, int);
    const KeyConfig& getP2keys() const;
    void goBack();
    void quit();

private:
    StateManager();
    StateManager(StateManager const&) = delete;
    void operator=(StateManager const&) = delete;
    State* _currentState;
    State* _newState;
    bool _switchState;
    SDLContext& SDL;
    InputManager& input;
    uint32_t _lastTick;
    uint32_t _frameTime;
    bool _running;
    uint32_t _framesRun;
    uint32_t _startTime;
    uint32_t _lastFrame;
    float _avgFps;
    bool _showFps;
    void showFps();
    KeyConfig _p1keys, _p2keys;
};

#endif /* STATEMANAGER_H_ */
