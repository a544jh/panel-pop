/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Config/ConfigHandler.h"
#include "Debug/DebugConsole.h"
#include "Debug/DebugOptions.h"
#include "SDLContext.h"
#include "States/StateManager.h"

int main(int argc, char *args[]) {
    DebugOptions &options = DebugOptions::get();
    options.parse(argc, args);

    if (options.usageRequested()) {
        std::cout << DebugOptions::usage();
        return 0;
    }

    if (options.headless()) {
        // Has to happen before SDL_Init: no window on screen, no sound device.
        SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
        SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
    }

    // A fixed seed (debug option) makes a scripted session reproducible: the
    // same blocks get dealt, so two AI runs can be compared fairly.
    const int seed = DebugOptions::get().seed();
    srand(seed != 0 ? static_cast<unsigned>(seed) : static_cast<unsigned>(time(NULL)));
    SDLContext &SDL = SDLContext::getInstance();
    ConfigHandler &configHandler = ConfigHandler::getInstance();

    configHandler.loadConfig();

    if (!SDL.init()) {
        std::cout << "initialization failed, aborting. Run with --help for the "
                     "headless/debug options." << std::endl;
        return 1;
    }

    StateManager &sm = StateManager::getInstance();
    if (options.console()) {
        DebugConsole::run();
    } else {
        sm.run();
    }

    SDL.tearDown();

    return 0;
}
