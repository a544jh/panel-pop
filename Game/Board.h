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


class Board {

public:
	struct Tile {
		Tile();
		TileType type;
		Block b;
		GarbageBlock* g;

	};

	enum BoardState {
		RUNNING, PAUSED, GAME_OVER
	};

	Board();

	virtual ~Board();

	void tick();
	void inputMoveCursor(Direction);
	void inputSwapBlocks();
	void inputForceStackRaise();
	void spawnGarbage(int, int, int, int, GarbageBlockType);

	static const int BASE_EXPLOSION_TICKS = 61;
	static const int ADD_EXPL_TICKS = 9; //the total explosion time for a combo is 61 + 9 * n, where n is the  number of blocks
	static const int SWAP_DELAY = 3;
	static const int BOARD_HEIGHT = 24;
	static const int BOARD_WIDTH = 6;
	static const int FLOAT_TICKS = 12;
	static const int STACK_RAISE_STEPS = 32;

	bool hasActiveBlocks() const;
	const Tile& getBufferRow(int) const;
	int getChainCounter() const;
	int getCursorX() const;
	int getCursorY() const;
	int getStackOffset() const;
	bool isStackRaiseForced() const;
	int getStackRaiseTicks() const;
	int getStackRaiseTimer() const;
	BoardState getState() const;
	bool isTickChain() const;
	int getTickMatchCol() const;
	int getTickMatched() const;
	int getTickMatchRow() const;
	int getGraceTimer() const;
	const Tile& getTile(int, int) const;
	const std::list<GarbageBlock>& getGarbageBlocks() const;

private:

	Tile _tiles[BOARD_HEIGHT][BOARD_WIDTH];
	Tile _bufferRow[BOARD_WIDTH];
	std::list<GarbageBlock> _garbageBlocks;

	BoardState _state;

	int _cursorX, _cursorY;
	int _tickMatched; //how many blocks got matched this tick
	int _stackOffset;
	int _stackRaiseTicks;
	int _stackRaiseTimer;
	int _graceTimer;
	bool _stackRaiseForced;
	bool _activeBlocks;
	int _chainCounter;
	bool _tickChain; //true if a chain has occurred during the tick
	int _tickMatchRow;
	int _tickMatchCol;
	bool _blockOnTopRow;


	void initTick();
	void initializeTiles();
	void fillRandom();
	void fillBufferRow();
	bool activeBlocks();
	void matchBlocks();
	void handleMatchedBlocks();
	void handleBlockTimers();
	void swapBlocks(int, int);
	void clearTile(Tile&);
	void setChain(int, int);
	void handleFalling();
	void handleGarbageFalling();
	void raiseStack();
	bool matchTiles(int, int, int, int);
	bool blockCanFall(int, int);
	bool swappable(int, int);
};

#endif /* BOARD_H_ */
