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
#include "BoardEventHandler.h"
#include "../InputState.h"

class Game;

class BoardEventHandler;

enum TileType {
  AIR, BLOCK, GARBAGE
};

class Board {
 public:

  struct Tile {
    Tile();

    TileType type;
    Block b;
    // TODO: refactor to use lookup from Board's list instead of pointer, so we can copy this object...
    GarbageBlock *g;
    bool chain = false;
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

  Board();
  Board reset();
  void setEventHandler(BoardEventHandler *);
  BoardEventHandler *getEventHandler() const;

  virtual ~Board();

  void tick();
  void inputMoveCursor(Direction);
  void inputSwapBlocks();
  void inputForceStackRaise();
  void queueGarbage(bool, int, GarbageBlockType);
  void advanceCountdownState();

  static const int BASE_EXPLOSION_TICKS = 61;
  static const int
      ADD_EXPL_TICKS = 9; //the total explosion time for a combo is 61 + 9 * n, where n is the  number of blocks
  static const int SWAP_DELAY = 3;
  static const int BOARD_HEIGHT = 24;
  static const int TOP_ROW = 11;
  static const int BOARD_WIDTH = 6;
  static const int PANIC_HEIGHT = 9;
  static const int WARN_HEIGHT = 10;
  static const int FLOAT_TICKS = 12;
  static const int STACK_RAISE_STEPS = 32;
  static const int COUNTDOWN_TICKS = 188;

  bool hasActiveBlocks() const;
  const Tile &getBufferRow(int) const;
  bool getWarnColumn(int) const;
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
  bool isPanic() const;
  int getScore() const;
  unsigned int getTicksRun() const;
  const Tile &getTile(int row, int col) const;
  const std::list<GarbageBlock> &getGarbageBlocks() const;
  const std::list<GarbageSpawn> &getGarbageQueue() const;
  void win();

 private:
  Tile _tiles[BOARD_HEIGHT][BOARD_WIDTH];
  Tile _bufferRow[BOARD_WIDTH];
  std::list<GarbageBlock> _garbageBlocks;
  std::list<GarbageSpawn> _garbageQueue;

  BoardState _state;
  BoardEventHandler *_eventHandler;
  unsigned int _ticksRun;
  // TODO: replace unneccesary variables with derived data, for easier copying....
  int _garbageSpawnPositions[3];
  bool _warnColumns[BOARD_WIDTH];
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
  bool _panic;
  int _score;
  int _countdownState;
 public:
  int getCountdownState() const;

 private:

  void initTick();
  void fillRandom();
  void fillBufferRow();
  bool activeBlocks();
  void matchBlocks();
  void handleGarbageQueue();
  void handleMatchedBlocks();
  void handleTriggeredBlocks();
  void handleBlockTimers();
  void swapBlocks(int, int);
  void clearTile(Tile &);
  bool checkChain(int row, int col);
  void handleFalling();
  void handleGarbageFalling();
  void triggerNeighbors(int, int);
  void triggerGarbageNeighbors(GarbageBlock &, Tile &);
  void triggerTile(int, int, Tile &);
  void raiseStack();
  bool matchTiles(int, int, int, int);
  bool blockCanFall(int, int);
  bool blockOnRow(int);
  bool garbageBlockCanFall(GarbageBlock &);
  bool swappable(int, int);
  bool spawnGarbage(int, int, int, int, GarbageBlockType);
  void sendEvents();
  void chainScoring();
  void comboScoring();
  bool checkAllAirAbove(int row, int col);
};

#endif /* BOARD_H_ */
