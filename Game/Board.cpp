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
	if (type == TileType::BLOCK) {
		return b._color;
	}
	return BlockColor::COUNT;
}

void Board::matchBlocks() {
	auto setExplHor = [this](int startIndex, int row, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[row][i].b._state = EXPLODING;
		}
	};

	auto setExplVer = [this](int startIndex, int col, int matched) {
		for(int i = startIndex; i < startIndex + matched; i++) {
			this->_tiles[i][col].b._state = EXPLODING;
		}
	};

	for (int row = 0; row < 24; row++) {
		int matched = 1;
		int matchStartIndex = 0;
		for (int col = 1; col < 6; col++) {
			BlockColor c1 = _tiles[row][col].getColor();
			BlockColor c2 = _tiles[row][col - 1].getColor();
			if (c1 != BlockColor::COUNT && c1 == c2) {
				matched++;
			} else {
				if (matched >= 3) {
					setExplHor(matchStartIndex, row, matched);
				}
				matchStartIndex = col;
				matched = 1;
			}
		}
		if (matched >= 3) {
			setExplHor(matchStartIndex, row, matched);
		}
	}

	for (int col = 0; col < 6; col++) {
		int matched = 1;
		int matchStartIndex = 0;
		for (int row = 1; row < 24; row++) {
			BlockColor c1 = _tiles[row][col].getColor();
			BlockColor c2 = _tiles[row - 1][col].getColor();
			if (c1 != BlockColor::COUNT && c1 == c2) {
				matched++;
			} else {
				if (matched >= 3) {
					setExplVer(matchStartIndex, col, matched);
				}
				matchStartIndex = row;
				matched = 1;
			}
		}
		if (matched >= 3) {
			setExplVer(matchStartIndex, col, matched);
		}
	}
}

void Board::tick() {
	matchBlocks();
}

Board::~Board() {
	// TODO Auto-generated destructor stub
}

