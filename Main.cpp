/*
 * File:  Main.cpp
 * Author: axelw
 *
 * Created on: Aug 15, 2015
 *
 */


#include "SDLContext.h"
#include "States/StateManager.h"
#include "Config/ConfigHandler.h"
#include "Config/KeyConfig.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_scancode.h>

#include <cstdlib>
#include <ctime>
#include <iostream>


int main(int argc, char* args[]) {
	srand(time(NULL));
	SDLContext& SDL = SDLContext::getInstance();
	ConfigHandler& configHandler = ConfigHandler::getInstance();

	configHandler.loadConfig();

	SDL.init();
	StateManager& sm = StateManager::getInstance();
	sm.run();

	SDL.tearDown();

	return 0;
}
