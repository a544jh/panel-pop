/*
 * BoardEventHandler.h
 *
 *  Created on: 7.2.2016
 *      Author: axelw
 */

#ifndef GAME_BOARDEVENTHANDLER_H_
#define GAME_BOARDEVENTHANDLER_H_

#include "../Rendering/GameRenderer.h"

class BoardEventHandler {
public:
	BoardEventHandler(GameRenderer&, int, int);
	virtual ~BoardEventHandler();

	void endTick();

	void blockExplode(int x, int y, int stackOffset, int order, int chain);
	void cursorMove();
	void swap();
	void blockFall();
	void gbFall();
	void combo();
	void chain();
	void chainEnd(int chain);

private:
	GameRenderer& _gr;
	int _boardXPos, _boardYPos; //board position in window
	SDLContext& _SDLContext;

	bool _lastFrameCursorMove;
	bool _thisFrameCursorMove;
	bool _combo;
	bool _chain;
	bool _blockFall;
};

#endif /* GAME_BOARDEVENTHANDLER_H_ */
