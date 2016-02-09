/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */

#include <stdlib.h>
#include <SDL2/SDL_main.h>
#include <time.h>
#include <string>

#include "SDLContext.h"
#include "States/StateManager.h"


int main(int argc, char* args[]) {
	srand(time(NULL));
	SDLContext& SDL = SDLContext::getInstance();
	SDL.init();
	StateManager& sm = StateManager::getInstance();
	sm.run();

	SDL.tearDown();
	return 0;
}
