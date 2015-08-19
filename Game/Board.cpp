/*
 * Board.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Board.h"

#include <stdlib.h>
#include <time.h>
#include <list>
#include <bitset>

Board::Board() :
_cursorX(0),
_cursorY(0)
{
	initializeTiles();
	fillRandom();

}

void Board::initializeTiles() {
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 6; j++) {
			_tiles[i][j].type = AIR;
		}
	}
}

void Board::fillRandom() {
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 6; j++) {
			_tiles[i][j].type = BLOCK;
			_tiles[i][j].b = Block();
			std::list<int> colors;
			for (int k = 0; k < BlockColor::COUNT; k++) {
				colors.push_back(k);
			}
			colors.remove(_tiles[i][j - 1].b._color);
			colors.remove(_tiles[i - 1][j].b._color);
			//_tiles[i][j].b._color = static_cast<BlockColor>(rand() % BlockColor::COUNT);
			auto it = colors.begin();

			int r = (rand() % colors.size());
			for (int k = 0; k < r; k++) {
				it++;
			}
			_tiles[i][j].b._color = static_cast<BlockColor>(*it);
		}
	}
}

void Board::moveCursor(Direction d) {
	switch(d){
	case UP:
		_cursorY++;
		break;
	case DOWN:
		_cursorY--;
		break;
	case RIGHT:
		_cursorX++;
		break;
	case LEFT:
		_cursorX--;
		break;
	}
}

void Board::swapBlocks() {
	Tile tmp = _tiles[_cursorY][_cursorX];
	_tiles[_cursorY][_cursorX] = _tiles [_cursorY][_cursorX + 1];
	_tiles [_cursorY][_cursorX + 1] = tmp;
}

void Board::tick(){

}

Board::~Board() {
	// TODO Auto-generated destructor stub
}

