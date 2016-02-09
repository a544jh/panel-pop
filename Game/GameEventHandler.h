/*
 * GameEventHandler.h
 *
 *  Created on: 8.2.2016
 *      Author: axelw
 */

#ifndef GAME_GAMEEVENTHANDLER_H_
#define GAME_GAMEEVENTHANDLER_H_

#include <cstdint>

class SDLContext;

class GameEventHandler {
public:
	GameEventHandler();
	virtual ~GameEventHandler();

	void countdown(uint32_t time);
	void gameReset();
	void gamePause();
	void gameResume();
	void panicBegin();
	void panicEnd();
	void tickEnd();

private:
	SDLContext& _SDLContext;
	int _countdownState;
	bool _songIntro;

};

#endif /* GAME_GAMEEVENTHANDLER_H_ */
