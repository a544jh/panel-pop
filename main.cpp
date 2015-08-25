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

int main(int argc, char* args[]) {
	SDLContext& SDL = SDLContext::getInstance();
	InputManager& input = InputManager::getInstance();
	SDL.init();
	Game game;
	GameRenderer gr = GameRenderer(game);

	while (!input._quit) {
		input.poll();

		game.tick();

		SDL_Texture* t = gr.renderGame();
		SDL.renderTextureToWindow(t);
		//SDL_Delay(50);
	}

	SDL.tearDown();
}
