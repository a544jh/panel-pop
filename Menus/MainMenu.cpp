/*
 * MainMenu.cpp
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#include "MainMenu.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>

#include "../SDLContext.h"
#include "../States/EndlessGameState.h"
#include "../States/OptionsMenuState.h"
#include "../States/StateManager.h"
#include "../States/VsGameState.h"
#include "../States/AIGameState.h"
#include "MenuItem.h"

MainMenu::MainMenu() {
    addItem(
        MenuItem("1P Endless",
                 [&]() {
                   StateManager::getInstance().switchToState(new EndlessGameState);
                 }));
    addItem(
        MenuItem("VS AI",
                 [&]() {
                   StateManager::getInstance().switchToState(new AIGameState);
                 }));
    addItem(
        MenuItem("2P VS",
                 [&]() {
                   StateManager::getInstance().switchToState(new VsGameState);
                 }));
    addItem(
        MenuItem("Options",
                 [&]() {
                   StateManager::getInstance().switchToState(new OptionsMenuState);
                 }));
    addItem(
        MenuItem("Quit",
                 [&]() {
                   StateManager::getInstance().quit();
                 }));
}

MainMenu::~MainMenu() {
}

void MainMenu::render() const {

    int x = 311;
    int y = 200;

    for (unsigned i = 0; i < _items.size(); ++i) {
        y += 18;
        std::string text = _items.at(i).getName();
        if (_selection == i && (SDL_GetTicks() - _time) % 1000 < 500) {
            text = "\u25b6" + text;
        } else {
            text = " " + text;
        }
        _SDLContext.renderText(text, {0, 0, 0}, _SDLContext._fontPs, x, y);

    }
}
