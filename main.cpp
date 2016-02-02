/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */
#include <stdlib.h>
#include <time.h>

#include "SDLContext.h"
#include "Game/Game.h"
#include "Rendering/GameRenderer.h"
#include "InputManager.h"
#include "States/StateManager.h"

int main(int argc, char* args[]) {
	setlocale(LC_ALL,"");
	srand(time(NULL));
	SDLContext& SDL = SDLContext::getInstance();
	InputManager& input = InputManager::getInstance();
	SDL.init();
	StateManager& sm = StateManager::getInstance();
	sm.run();

	SDL.tearDown();
}
