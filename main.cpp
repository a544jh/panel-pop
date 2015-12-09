/*
 * main.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: axel
 */
#include "SDLContext.h"
#include "Game/Game.h"
#include "Rendering/GameRenderer.h"
#include "InputManager.h"
#include "States/StateManager.h"

int main(int argc, char* args[]) {
	SDLContext& SDL = SDLContext::getInstance();
	InputManager& input = InputManager::getInstance();
	SDL.init();
	StateManager& sm = StateManager::getInstance();
	sm.run();

	SDL.tearDown();
}
