/*
 * BoardEventHandler.cpp
 *
 *  Created on: 7.2.2016
 *      Author: axelw
 */

#include "BoardEventHandler.h"

#include "../Rendering/BoardRenderer.h"
#include "../Rendering/ComboPopup.h"

BoardEventHandler::BoardEventHandler(GameRenderer& gr, int x, int y) :
				_gr(gr),
				_boardXPos(x),
				_boardYPos(y) {

}

BoardEventHandler::~BoardEventHandler() {
}

void BoardEventHandler::blockExplode(int x, int y, int stackOffset, int combo, int chain) {
	int posx = x * BoardRenderer::TILE_SIZE + 5 + _boardXPos;
	int posy = BoardRenderer::BOARD_HEIGHT
			- (y + 1) * BoardRenderer::TILE_SIZE
			- stackOffset + _boardYPos;
	_gr.addPopup(new ComboPopup(posx,posy,99,100)); // TODO: change to particle
}
