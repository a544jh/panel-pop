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
	Tile();

	TileType type;
	Block b;
	GarbageBlock* g;
	bool swappable();
};

class Board {
public:
	Board();
	Tile _tiles[24][6];
	Tile _bufferRow[6];
	virtual ~Board();
	int _cursorX, _cursorY;
	int _tickMatched; //how many blocks got matched this tick
	int _stackOffset;
	int _stackRaiseTicks;
	int _stackRaiseTimer;
	bool _stackRaiseForced;
	bool _activeBlocks;
	void tick();
	void moveCursor(Direction);
	void swapBlocks();
	void forceStackRaise();
	static const int BASE_EXPLOSION_TICKS = 61;
	static const int ADD_EXPL_TICKS = 9; //the total explosion time for a combo is 61 + 9 * n, where n is the  number of blocks
	static const int BOARD_HEIGHT = 24;
	static const int BOARD_WIDTH = 6;
private:
	void initTick();
	void initializeTiles();
	void fillRandom();
	void fillBufferRow();
	bool activeBlocks();
	void matchBlocks();
	void setExplosionTicks();
	void handleExplodingBlocks();
	void deleteBlock(Tile&);
	void handleFalling();
	void raiseStack();
	bool matchTiles(int, int, int, int);
	bool blockCanFall(int, int);

	static const int SWAP_FLOAT_TICKS = 12;
};

#endif /* BOARD_H_ */
