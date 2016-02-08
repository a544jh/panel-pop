/*
 * Board.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <cstdint>
#include <list>

#include "Block.h"
#include "GarbageBlock.h"

class BoardEventHandler;

class Game;

enum Direction {
	UP, RIGHT, DOWN, LEFT, NONE
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

	struct GarbageSpawn {
		bool fullWidth;
		GarbageBlockType type;
		int size;
		int spawnTimer;
	};

	enum BoardState {
		RUNNING, COUNTDOWN, WON, GAME_OVER
	};

	Board(Game*, BoardEventHandler*);

	virtual ~Board();

	void tick();
	void inputMoveCursor(Direction);
	void inputSwapBlocks();
	void inputForceStackRaise();
	void queueGarbage(bool, int, GarbageBlockType);

	static const int BASE_EXPLOSION_TICKS = 61;
	static const int ADD_EXPL_TICKS = 15; //the total explosion time for a combo is 61 + 9 * n, where n is the  number of blocks
	static const int SWAP_DELAY = 3;
	static const int BOARD_HEIGHT = 24;
	static const int BOARD_WIDTH = 6;
	static const int FLOAT_TICKS = 12;
	static const int STACK_RAISE_STEPS = 32;
	static const int COUNTDOWN_MS = 3000;

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
	bool isTickChainEnd() const;
	int getTickMatchCol() const;
	int getTickMatched() const;
	int getTickMatchRow() const;
	int getGraceTimer() const;
	int getLastChain() const;
	unsigned int getTicksRun() const;
	const Tile& getTile(int, int) const;
	const std::list<GarbageBlock>& getGarbageBlocks() const;
	const std::list<GarbageSpawn>& getGarbageQueue() const;
	void win();
	Game& getGame() const;
	uint32_t getTime() const;

private:
	Tile _tiles[BOARD_HEIGHT][BOARD_WIDTH];
	Tile _bufferRow[BOARD_WIDTH];
	std::list<GarbageBlock> _garbageBlocks;
	std::list<GarbageSpawn> _garbageQueue;

	Game* _game;
	BoardState _state;
	BoardEventHandler* _eventHandler;
	unsigned int _ticksRun;
	int _garbageSpawnPositions[3];
	int _cursorX, _cursorY;
	int _tickMatched; //how many blocks got matched this tick
	int _stackOffset;
	int _stackRaiseTicks; //ticks to raise stack one step
	int _stackRaiseTimer;
	bool _stackRaiseForced;
	int _graceTimer;
	bool _activeBlocks;
	int _chainCounter;
	bool _tickChain; //true if a chain has occurred during the tick
	bool _tickChainEnd; //true if chain ended on this tick
	int _lastChain; //size of last chain
	int _tickMatchRow; //location of the match
	int _tickMatchCol;
	bool _blockOnTopRow;

	void initTick();
	void initializeTiles();
	void fillRandom();
	void fillBufferRow();
	bool activeBlocks();
	void matchBlocks();
	void handleGarbageQueue();
	void handleMatchedBlocks();
	void handleTriggeredBlocks();
	void handleBlockTimers();
	void swapBlocks(int, int);
	void clearTile(Tile&);
	void setChainAbove(int, int);
	void handleFalling();
	void handleGarbageFalling();
	void triggerNeighbors(int, int);
	void triggerGarbageNeighbors(GarbageBlock&, Tile&);
	void triggerTile(int, int, Tile&);
	void raiseStack();
	bool matchTiles(int, int, int, int);
	bool blockCanFall(int, int);
	bool garbageBlockCanFall(GarbageBlock&);
	bool swappable(int, int);
	bool spawnGarbage(int,int,int,int, GarbageBlockType);
};

#endif /* BOARD_H_ */
