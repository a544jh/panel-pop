/*
 * EndlessGame.cpp
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#include "EndlessGame.h"

#include "GameEventHandler.h"

class BoardEventHandler;

EndlessGame::EndlessGame(GameEventHandler* geh, BoardEventHandler* beh) :
				Game(geh),
				_board(this, beh) {

}

EndlessGame::~EndlessGame() {

}

void EndlessGame::reset() {
}

void EndlessGame::tick() {
}

void EndlessGame::handleEnd() {
}
