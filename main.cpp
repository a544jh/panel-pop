/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include <stdlib.h>
#include <SDL2/SDL_main.h>
#include <time.h>
#include <exception>
#include <iostream>

#include "Config/ConfigHandler.h"
#include "SDLContext.h"
#include "States/StateManager.h"

int main(int argc, char* args[]) {
	//srand(time(NULL));
	//SDLContext& SDL = SDLContext::getInstance();
	ConfigHandler& configHandler = ConfigHandler::getInstance();

	try {
		configHandler.loadConfig();
	} catch(...) {
		std::cout << "error";
	}

	//SDL.init();
	//StateManager& sm = StateManager::getInstance();
	//sm.run();

	//SDL.tearDown();

//configHandler.saveConfig();

	return 0;
}
