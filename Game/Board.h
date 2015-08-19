/*
 * Board.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "Block.h"
#include "GarbageBlock.h"

enum Direction {
	UP, RIGHT, DOWN, LEFT
};

enum TileType {
	AIR, BLOCK, GARBAGE
};

struct Tile {
	TileType type;
	Block b;
	GarbageBlock* g;
	BlockColor getColor();
};

class Board {
public:
	Board();
	Tile _tiles[24][6];
	virtual ~Board();
	int _cursorX, _cursorY;
	void tick();
	void moveCursor(Direction);
	void swapBlocks();
private:
	void initializeTiles();
	void fillRandom();
	void matchBlocks();
};

#endif /* BOARD_H_ */
