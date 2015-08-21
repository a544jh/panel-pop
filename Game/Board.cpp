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
	fillRandom();

}

Tile::Tile() :
		type(AIR), g(nullptr) {
}

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
	while (_cursorX > BOARD_WIDTH - 2) {
		--_cursorX;
	}
	while (_cursorY > 11) {
		--_cursorY;
	}
	while (_cursorX < 0) {
		++_cursorX;
	}
	while (_cursorY < 0) {
		++_cursorY;
	}
}

bool Tile::swappable() {
	return (type == AIR || (type == BLOCK && b._state == NORMAL));
}

bool Board::blockCanFall(int row, int col) {
	if (row == 0 || _tiles[row][col].b._state != NORMAL) {
		return false;
	} else {
		return _tiles[row - 1][col].type == AIR;
	}
}

bool Board::matchTiles(int r1, int c1, int r2, int c2) {
	Tile& t1 = _tiles[r1][c1];
	Tile& t2 = _tiles[r2][c2];
	return (t1.type == BLOCK && t2.type == BLOCK
			&& (t1.b._state == NORMAL || t1.b._state == MATCHED)
			&& (t2.b._state == NORMAL || t2.b._state == MATCHED)
			&& !t1.b._falling && !t2.b._falling && t1.b._color == t2.b._color);
}

void Board::swapBlocks() {
	Tile& t1 = _tiles[_cursorY][_cursorX];
	Tile& t2 = _tiles[_cursorY][_cursorX + 1];
	if (!(t1.swappable() && t2.swappable())) {
		return;
	}
	Tile tmp = t1;
	t1 = t2;
	t2 = tmp;

	if (blockCanFall(_cursorY, _cursorX)) {
		t1.b._state = FLOATING;
		t1.b._floatTimer = SWAP_FLOAT_TICKS;
	}
	if (blockCanFall(_cursorY, _cursorX + 1)) {
		t2.b._state = FLOATING;
		t2.b._floatTimer = SWAP_FLOAT_TICKS;
	}
}

void Board::initTick() {
	_tickMatched = 0;
}

void Board::matchBlocks() {

	auto setMatchedHor = [this](int startIndex, int row, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[row][i].b._state = MATCHED;
			++_tickMatched;
		}
	};

	auto setMatchedVer = [this](int startIndex, int col, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[i][col].b._state = MATCHED;
			++_tickMatched;
		}
	};

	for (int row = 0; row < BOARD_HEIGHT; row++) {
		int matched = 1;
		int matchStartIndex = 0;
		for (int col = 1; col < BOARD_WIDTH; col++) {
			if (matchTiles(row, col, row, col - 1)) {
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
			if (matchTiles(row, col, row - 1, col)) {
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
		for (int col = 0; col < BOARD_WIDTH; col++) {
			Tile& tile = _tiles[row][col];
			if (tile.type == BLOCK && tile.b._state == MATCHED) {
				tile.b._state = EXPLODING;
				tile.b._explosionTimer = 0;
				tile.b._explosionAnimTicks = BASE_EXPLOSION_TICKS
						+ matches * ADD_EXPL_TICKS;
				tile.b._explosionTicks = BASE_EXPLOSION_TICKS
						+ _tickMatched * ADD_EXPL_TICKS;
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
			if (it->b._explosionTicks == it->b._explosionTimer) {
				deleteBlock(*it);
			}
		}
	}
}

void Board::handleFalling() {
	for (int col = 0; col < BOARD_WIDTH; col++) {
		for (int row = 0; row < BOARD_HEIGHT; row++) {
			if (blockCanFall(row, col)) {
				_tiles[row - 1][col] = _tiles[row][col];
				_tiles[row - 1][col].b._falling = true;
				deleteBlock(_tiles[row][col]);
			} else {
				if(_tiles[row][col].b._state == FLOATING){
					if(_tiles[row][col].b._floatTimer-- <= 0){
						_tiles[row][col].b._state = NORMAL;
					}
				}
				_tiles[row][col].b._falling = false;
			}
		}
	}
}

void Board::tick() {
	initTick();
	handleExplodingBlocks();
	handleFalling();
	matchBlocks();
	setExplosionTicks();
}

Board::~Board() {
	// TODO Auto-generated destructor stub
}

