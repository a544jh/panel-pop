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
#include <iostream>

Board::Board() :
		_cursorX(0), _cursorY(0), _stackOffset(0), _stackRaiseTicks(10), _stackRaiseTimer(
				0), _stackRaiseForced(false), _chainCounter(1), _tickChain(
				false) {
	fillRandom();
	fillBufferRow();
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

void Board::fillBufferRow() {
	srand(time(NULL));
	for (int i = 0; i < BOARD_WIDTH; i++) {
		std::list<int> colors;
		for (int k = 0; k < BlockColor::COUNT; k++) {
			colors.push_back(k);
		}
		colors.remove(_tiles[0][i].b._color);
		colors.remove(_bufferRow[i - 1].b._color);
		auto it = colors.begin();

		int r = (rand() % colors.size());
		for (int k = 0; k < r; k++) {
			it++;
		}
		_bufferRow[i].type = BLOCK;
		_bufferRow[i].b._color = static_cast<BlockColor>(*it);
	}
}

void Board::inputMoveCursor(Direction d) {

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

bool Board::swappable(int row, int col) {
	//prevent block from swapping into empty space with a block above it
	Tile& t = _tiles[row][col];
	return ((t.type == AIR && _tiles[row + 1][col].type == AIR)
			|| (t.type == BLOCK && t.b._state == NORMAL));
}

bool Board::blockCanFall(int row, int col) {
	if (_tiles[row][col].type != BLOCK || row == 0
			|| _tiles[row][col].b._state != NORMAL) {
		return false;
	} else {
		return (_tiles[row - 1][col].type == AIR
				&& _tiles[row - 1][col].b._state == NORMAL);
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

void Board::inputSwapBlocks() {
	if (!(swappable(_cursorY, _cursorX) && swappable(_cursorY, _cursorX + 1))) {
		return;
	}
	_tiles[_cursorY][_cursorX].b._state = SWAPPING_RIGHT;
	_tiles[_cursorY][_cursorX + 1].b._state = SWAPPING_LEFT;
}

void Board::initTick() {
	_tickMatched = 0;
	_tickChain = false;
	_activeBlocks = activeBlocks();
	if (!_activeBlocks) {
		_chainCounter = 1;
	}
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
void Board::handleMatchedBlocks() {
	int matches = 1;
	bool chain = false;
	bool match = false;
	for (int row = BOARD_HEIGHT; row >= 0; row--) {
		for (int col = 0; col < BOARD_WIDTH; col++) {
			Tile& tile = _tiles[row][col];

			if (tile.type == BLOCK && tile.b._state == MATCHED) {
				if (tile.b._chain && !chain) {
					chain = true;
					_tickChainCol = col;
					_tickChainRow = row;
				}
				if (!match) {
					match = true;
					_tickMatchCol = col;
					_tickMatchRow = row;
				}
				tile.b._state = EXPLODING;
				tile.b._explosionTimer = 0;
				tile.b._explosionAnimTicks = BASE_EXPLOSION_TICKS
						+ matches * ADD_EXPL_TICKS;
				tile.b._explosionTicks = BASE_EXPLOSION_TICKS
						+ _tickMatched * ADD_EXPL_TICKS;
				++matches;
			}
			if (tile.b._state == NORMAL && !tile.b._falling && tile.b._chain) {
				tile.b._chain = false;
			}
		}
	}
	if (chain) {
		++_chainCounter;
		_tickChain = true;
	}
}

void Board::deleteBlock(Tile& tile) {
	tile = Tile();
}

void Board::handleBlockTimers() {
	for (int row = 0; row < BOARD_HEIGHT; ++row) {
		for (int col = 0; col < BOARD_WIDTH; ++col) {
			Tile& tile = _tiles[row][col];
			if (tile.type == BLOCK && tile.b._state == EXPLODING) {
				++tile.b._explosionTimer;
				if (tile.b._explosionTicks == tile.b._explosionTimer) {
					deleteBlock(tile);
					// we need to set the chain flag for the blocks above, and set it on the others above when it's about to fall
					if (_tiles[row + 1][col].type == BLOCK
							&& _tiles[row + 1][col].b._state == NORMAL) {
						_tiles[row + 1][col].b._chain = true;
					}
				}
			}
			if (tile.b._state == SWAPPING_LEFT
					|| tile.b._state == SWAPPING_RIGHT) {
				tile.b._swapTimer++;
				if (tile.b._swapTimer >= SWAP_DELAY) {
					if (tile.b._state == SWAPPING_RIGHT) {
						swapBlocks(row, col);
					}
				}
			}
		}
	}
}

void Board::swapBlocks(int row, int col) {
	Tile& swapRight = _tiles[row][col];
	Tile& swapLeft = _tiles[row][col + 1];
	if (swapLeft.b._state == SWAPPING_LEFT) {
		Tile tmp = swapLeft;
		swapLeft = swapRight;
		swapRight = tmp;
	} else {
		std::cout << "CAN'T SWAP!!\n";
	}
	swapRight.b._swapTimer = 0;
	swapRight.b._state = NORMAL;
	swapLeft.b._swapTimer = 0;
	swapLeft.b._state = NORMAL;
}

//sets the chain flag for the block and the ones above it
void Board::setChain(int row, int col) {
	for (int y = row; y < BOARD_HEIGHT; ++y) {
		Tile& tile = _tiles[y][col];
		if (tile.type == BLOCK) {
			tile.b._chain = true;
		}
	}
}

void Board::handleFalling() {
	for (int col = 0; col < BOARD_WIDTH; col++) {
		for (int row = 0; row < BOARD_HEIGHT; row++) {
			Tile& tile = _tiles[row][col];
			if (tile.b._state == FLOATING) { //floating->falling
				if (--tile.b._floatTimer <= 0) {
					tile.b._state = NORMAL;
					tile.b._falling = true;
					for (int y = row; y < BOARD_HEIGHT; ++y) {
						if (_tiles[y][col].type == BLOCK) {
							_tiles[y][col].b._falling = true;
						}
					}
					if (tile.b._chain) {
						setChain(row, col);
					}
				}
			}
			if (blockCanFall(row, col)) {
				if (!tile.b._falling) { //normal -> floating
					tile.b._state = FLOATING;
					tile.b._floatTimer = FLOAT_TICKS;
				} else { //actual falling
					_tiles[row - 1][col] = _tiles[row][col];
					deleteBlock(_tiles[row][col]);
				}
			} else {
				tile.b._falling = false;
			}
		}
	}
}

void Board::raiseStack() {
	if (!_stackRaiseForced && _stackRaiseTimer < _stackRaiseTicks) {
		++_stackRaiseTimer;
		return;
	}
	if (_stackOffset < 32 && !_activeBlocks) {
		++_stackOffset;
		_stackRaiseTimer = 0;
	}
	if (_stackOffset >= 32) {
		for (int row = BOARD_HEIGHT - 2; row >= 0; --row) {
			for (int col = 0; col < BOARD_WIDTH; ++col) {
				_tiles[row + 1][col] = _tiles[row][col];
			}
		}
		for (int i = 0; i < BOARD_WIDTH; ++i) {
			_tiles[0][i] = _bufferRow[i];
		}
		fillBufferRow();
		_stackOffset = 0;
		_stackRaiseForced = false;
		if (_cursorY <= 10) {
			++_cursorY;
		}
	}
}

void Board::inputForceStackRaise() {
	if (!_activeBlocks) {
		_stackRaiseForced = true;
	}
}

bool Board::activeBlocks() {
	for (Tile* it = &_tiles[0][0];
			it != &_tiles[0][0] + BOARD_WIDTH * BOARD_HEIGHT; ++it) {
		if (it->type == BLOCK && (it->b._state != NORMAL || it->b._falling)) {
			return true;
		}
	}
	return false;
}

void Board::tick() {
	initTick();
	raiseStack();
	handleBlockTimers();
	handleFalling();
	matchBlocks();
	handleMatchedBlocks();
}

Board::~Board() {
// TODO Auto-generated destructor stub
}

