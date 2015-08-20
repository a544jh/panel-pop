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
		_cursorX(0), _cursorY(0) {
	//initializeTiles();
	fillRandom();

}

Tile::Tile() :
		type(AIR), g(nullptr) {
}

//void Board::initializeTiles() {
//	for (int i = 0; i < BOARD_HEIGHT; i++) {
//		for (int j = 0; j < BOARD_WIDTH; j++) {
//			_tiles[i][j].type = AIR;
//		}
//	}
//}

void Board::fillRandom() {
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
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
	//TODO: Don't move if cursor gets out of bounds
	switch (d) {
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
	_tiles[_cursorY][_cursorX] = _tiles[_cursorY][_cursorX + 1];
	_tiles[_cursorY][_cursorX + 1] = tmp;
}

BlockColor Tile::getColor() {
	if (type == TileType::BLOCK && b._state == NORMAL) {
		return b._color;
	}
	return BlockColor::COUNT;
}

void Board::initTick() {
	_tickMatched = 0;
}

void Board::matchBlocks() {

	auto setMatchedHor = [this](int startIndex, int row, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[row][i].b._state = MATCHED;
		}
	};

	auto setMatchedVer = [this](int startIndex, int col, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[i][col].b._state = MATCHED;
		}
	};

	for (int row = 0; row < BOARD_HEIGHT; row++) {
		int matched = 1;
		int matchStartIndex = 0;
		for (int col = 1; col < BOARD_WIDTH; col++) {
			BlockColor c1 = _tiles[row][col].getColor();
			BlockColor c2 = _tiles[row][col - 1].getColor();
			if (c1 != BlockColor::COUNT && c1 == c2) {
				matched++;
			} else {
				if (matched >= 3) {
					setMatchedHor(matchStartIndex, row, matched);
				}
				matchStartIndex = col;
				matched = 1;
			}
		}
		if (matched >= 3) {
			setMatchedHor(matchStartIndex, row, matched);
		}
	}

	for (int col = 0; col < BOARD_WIDTH; col++) {
		int matched = 1;
		int matchStartIndex = 0;
		for (int row = 1; row < BOARD_HEIGHT; row++) {
			BlockColor c1 = _tiles[row][col].getColor();
			BlockColor c2 = _tiles[row - 1][col].getColor();
			if (c1 != BlockColor::COUNT && c1 == c2) {
				matched++;
			} else {
				if (matched >= 3) {
					setMatchedVer(matchStartIndex, col, matched);
				}
				matchStartIndex = row;
				matched = 1;
			}
		}
		if (matched >= 3) {
			setMatchedVer(matchStartIndex, col, matched);
		}
	}
}

//scan the board from top left and set the explosion time for each block, so we get a pretty animation
void Board::setExplosionTicks() {
	int matches = 1;
	for (int row = BOARD_HEIGHT; row >= 0; row--) {
		for (int col = BOARD_WIDTH; col >= 0; col--) {
			Tile& tile = _tiles[row][col];
			if (tile.type == BLOCK && tile.b._state == MATCHED) {
				tile.b._state = EXPLODING;
				tile.b._explosionTimer = 0;
				tile.b._explosionAnimTicks = BASE_EXPLOSION_TICKS
						+ matches * ADD_EXPL_TICKS;
				tile.b._explosionTicks = BASE_EXPLOSION_TICKS + _tickMatched * ADD_EXPL_TICKS;
				++matches;
			}
		}
	}
}

void Board::deleteBlock(Tile& tile) {
	tile = Tile();
}

void Board::handleExplodingBlocks() {
	for (Tile* it = &_tiles[0][0];
			it != &_tiles[0][0] + BOARD_WIDTH * BOARD_HEIGHT; ++it) {
		if (it->type == BLOCK && it->b._state == EXPLODING) {
			++it->b._explosionTimer;
			it->b._animBlinkState = !it->b._animBlinkState;
		}
		if (it->b._explosionTicks == it->b._explosionTimer) {
			deleteBlock(*it);
		}
	}
}

void Board::tick() {
	initTick();
	handleExplodingBlocks();
	matchBlocks();
	setExplosionTicks();
}

Board::~Board() {
	// TODO Auto-generated destructor stub
}

