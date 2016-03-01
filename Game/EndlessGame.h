/*
 * EndlessGame.h
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#ifndef GAME_ENDLESSGAME_H_
#define GAME_ENDLESSGAME_H_

#include "Board.h"
#include "Game.h"

class EndlessGame: public Game {
public:
	EndlessGame(GameEventHandler*, BoardEventHandler*);
	virtual ~EndlessGame();
	void reset();
	void tick();
	Board& getBoard(int);
	int getHighScore() const;

private:
	Board _board;
	int _highScore;
	void handleEnd();
	BoardEventHandler* _boardEventHandler;
};

#endif /* GAME_ENDLESSGAME_H_ */
