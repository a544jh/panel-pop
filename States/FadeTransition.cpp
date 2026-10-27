
/* 
 * File:   FadeTransition.cpp
 * Author: axel
 * 
 * Created on October 9, 2016, 10:33 PM
 */

#include <climits>
#include "FadeTransition.h"
#include "StateManager.h"
#include "../SDLContext.h"

FadeTransition::FadeTransition(StateConstructor constr) : _nextStateConstr(constr),
                                                          _ticksRun(0), _nextState(nullptr), _lastTarget(nullptr) {
    StateManager &manager = StateManager::getInstance();
}

void FadeTransition::tick() {
    if (_nextState != nullptr && _ticksRun >= TRANSITION_TICKS / 2) {
        _nextState->tick();
    }
    if (++_ticksRun >= TRANSITION_TICKS) {
        StateManager::getInstance().switchToState(std::move(_nextState));
    }
}

SDL_Texture *FadeTransition::render() {
    if (_nextState == nullptr && _ticksRun >= TRANSITION_TICKS) {
        //the state was already handed off to StateManager (the swap happens after
        //rendering); keep showing its last frame, the fade overlay is fully transparent anyway
        return _lastTarget;
    }
    SDL_Renderer *renderer = SDLContext::getInstance().getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    double fadeout;
    SDL_Texture *target;
    if (_ticksRun < TRANSITION_TICKS / 2) {
        target = NULL;
        fadeout = _ticksRun / (double) (TRANSITION_TICKS / 2);
    } else {
        if (_nextState == nullptr) {
            _nextState = _nextStateConstr();
        }
        target = _nextState->render();
        fadeout = 1 - ((_ticksRun - TRANSITION_TICKS / 2) / (double) (TRANSITION_TICKS / 2));
    }
    SDL_SetRenderTarget(renderer, target);
    uint8_t alpha = 0xFF * fadeout;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_Rect rect = {0, 0, INT_MAX, INT_MAX};
    SDL_RenderFillRect(renderer, &rect);

    _lastTarget = target;
    return target;
}

void FadeTransition::goBack() {

}

FadeTransition::~FadeTransition() {
}

