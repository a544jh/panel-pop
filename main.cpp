/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_scancode.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Config/ConfigHandler.h"
#include "SDLContext.h"
#include "States/StateManager.h"

int main(int argc, char *args[]) {
    srand(time(NULL));
    SDLContext &SDL = SDLContext::getInstance();
    ConfigHandler &configHandler = ConfigHandler::getInstance();

    configHandler.loadConfig();

    SDL.init();

    StateManager &sm = StateManager::getInstance();
    sm.run();

    SDL.tearDown();

    return 0;
}
