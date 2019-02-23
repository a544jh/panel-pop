
/* 
 * File:   FadeTransition.cpp
 * Author: axel
 * 
 * Created on October 9, 2016, 10:33 PM
 */

#include "FadeTransition.h"
#include "StateManager.h"
#include "../SDLContext.h"

FadeTransition::FadeTransition(StateConstructor constr) : _nextStateConstr(constr),
_ticksRun(0), _nextState(nullptr) {
    StateManager& manager = StateManager::getInstance();
    _prevTexture = manager.getCurrentState()->render();
}

void FadeTransition::tick() {
    if (++_ticksRun >= TRANSITION_TICKS) {
        StateManager::getInstance().switchToState(_nextState);
    }
}

SDL_Texture* FadeTransition::render() {
    SDL_Renderer* renderer = SDLContext::getInstance().getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect = {0, 0, SDLContext::WINDOW_WIDTH, SDLContext::WINDOW_HEIGHT};
    double fadeout;
    SDL_Texture* target;
    if (_ticksRun < TRANSITION_TICKS / 2) {
        target = _prevTexture;
        fadeout = _ticksRun / (double) (TRANSITION_TICKS / 2);
    } else {
        if (_nextState == nullptr) {
            _nextState = _nextStateConstr();
        }
        _nextState->tick();
        target = _nextState->render();
        fadeout = 1 - ((_ticksRun - TRANSITION_TICKS / 2) / (double) (TRANSITION_TICKS / 2));
    }
    SDL_SetRenderTarget(renderer, target);
    uint8_t alpha = 0xFF * fadeout;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(renderer, &rect);

    return target;
}

void FadeTransition::goBack() {

}

FadeTransition::~FadeTransition() {
}

