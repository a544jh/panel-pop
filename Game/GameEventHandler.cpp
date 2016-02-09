/*
 * GameEventHandler.cpp
 *
 *  Created on: 8.2.2016
 *      Author: axelw
 */

#include "GameEventHandler.h"

#include <SDL2/SDL_mixer.h>

#include "../SDLContext.h"

GameEventHandler::GameEventHandler() :
				_SDLContext(SDLContext::getInstance()),
				_countdownState(-1) {

}

GameEventHandler::~GameEventHandler() {
}

void GameEventHandler::countdown(uint32_t time) {
	int sec = time / 1000;
	if (sec != _countdownState) {
		if (sec < 3) {
			Mix_PlayChannel(-1, _SDLContext._sfxCountdown, 0);
		} else {
			Mix_PlayChannel(-1, _SDLContext._sfxGo, 0);
			Mix_PlayMusic(_SDLContext._musicBg, -1);
		}
		_countdownState = sec;
	}

}

void GameEventHandler::gameReset() {
	_countdownState = -1;
	Mix_HaltMusic();
}

void GameEventHandler::gamePause() {
	Mix_PauseMusic();
	Mix_PlayChannel(-1, _SDLContext._sfxPause, 0);
}

void GameEventHandler::gameResume() {
	Mix_ResumeMusic();
}

void GameEventHandler::panicBegin() {
	Mix_PlayMusic(_SDLContext._musicPanic, -1);
}

void GameEventHandler::panicEnd() {
	Mix_PlayMusic(_SDLContext._musicBg, -1);
}
