/*
 * PauseMenu.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "PauseMenu.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>

#include "../Game/Game.h"
#include "../SDLContext.h"
#include "../States/StateManager.h"
#include "MenuItem.h"

PauseMenu::PauseMenu(Game &game) :
    _game(game) {
    addItem(MenuItem("RESUME", [this]() { _game.inputTogglePause(); }));
    addItem(
        MenuItem("QUIT",
                 []() { StateManager::getInstance().returnToTitle(); }));
}

PauseMenu::~PauseMenu() {
}

void PauseMenu::render() const {
    //background
    SDL_Rect sprite = {0, 410, 150, 70};
    SDL_Rect pos = {245, 205, 150, 70};
    SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);

    int x = pos.x + 20;
    int y = pos.y + 8;

    _SDLContext.renderText("-PAUSE-", {255, 255, 255}, _SDLContext._fontPs, x,
                           y);

    x = pos.x + 16;
    y += 2;

    for (unsigned i = 0; i < _items.size(); ++i) {
        y += 18;
        std::string text = _items.at(i).getName();
        if (_selection == i && (SDL_GetTicks() - _time) % 1000 < 500) {
            text = "\u25b6" + text;
        } else {
            text = " " + text;
        }
        _SDLContext.renderText(text, {255, 255, 255}, _SDLContext._fontPs, x,
                               y);
    }

}

void PauseMenu::inputCancel() {
    _game.inputTogglePause();
}
