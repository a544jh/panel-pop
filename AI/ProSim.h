/*
 * ProSim.h
 *
 * Board model for ProStrategy (the "pro" AI): a compact, copyable,
 * tick-exact re-implementation of Board::tick() and the board input
 * functions, plus the cursor, so a list of AIStrategyActions can be played
 * on it exactly as AIBoardController plays them on the real board.
 *
 * Exactness is the point. The strategy times swaps against falling blocks
 * (skill chains), so a model that is one tick off picks moves that miss.
 * Every rule follows Game/Board.cpp, including its quirks: the std::list
 * erase-and-continue loops in handleGarbageQueue()/handleBlockTimers(), the
 * double chain scoring of combos, the panic flag that only looks at row 9,
 * and so on. What it cannot know is the future: colours of rows that do not
 * exist yet come from the model's own PRNG (inventedColors() says when such
 * a colour is on the board), and garbage the opponent sends after import()
 * is not there.
 *
 * Kept separate from SimBoard (QwenStrat's model) on purpose; see AGENTS.md.
 */

#ifndef AI_PROSIM_H_
#define AI_PROSIM_H_

#include <cstdint>

#include "AIStrategy.h"
#include "../Game/Board.h"

class ProSim {
 public:
  static const int H = Board::BOARD_HEIGHT;
  static const int W = Board::BOARD_WIDTH;
  static const int MAX_GARBAGE = 32;
  static const int MAX_SPAWNS = 16;

  enum CellType {
    AIR = 0, BLOCK = 1, GARBAGE = 2
  };

  struct Cell {
    uint8_t type;      // CellType
    uint8_t color;     // BlockColor, COUNT for air/garbage
    uint8_t state;     // BlockState; also set on the AIR half of a swap
    uint8_t chain;     // Tile::chain, a property of the position
    uint8_t falling;   // Block::_falling
    uint8_t gbFall;    // Block::_gbFallChain
    int8_t g;          // garbage index for GARBAGE cells, else -1
    uint8_t swapTimer;
    int8_t floatTimer;
    uint16_t explTimer;
    uint16_t explTicks;
    uint16_t animTicks; // Block::_explosionAnimTicks: the +10 score moment
  };

  struct Garbage {
    int8_t x, y, w, h;   // y is the top row, as in GarbageBlock
    uint8_t type;        // GarbageBlockType
    uint8_t state;       // GarbageBlockState
    int16_t transTicks, transTimer;
    uint8_t falling, initialFall;
    uint8_t invented;    // color[] was made up by the model
    uint8_t color[W];    // colours of the next row to transform
  };

  struct Spawn {          // Board::GarbageSpawn
    uint8_t fullWidth;
    uint8_t type;
    int8_t size;
    int16_t timer;
  };

  // What happened since import()/resetStats(), for the evaluation.
  struct Stats {
    int maxChain;        // highest chain counter reached
    int chainRows;       // full-width garbage rows sent by chain ends
    int comboCells;      // garbage cells sent by combos (VsGame rules)
    int combos;          // ticks that matched 4+ blocks
    int blocksCleared;   // blocks that finished exploding
    int garbageCleared;  // garbage cells turned into blocks
    int links;           // chain counter increments
    int matches;         // ticks with at least one match
    int lastLinkTick;    // tick (since import) of the latest link, -1 if none
    int numChains;       // chains that ended (2+), lengths in chainLen[]
    int chainLen[8];
  };

  ProSim();

  // Copies everything from a live board, including the cursor.
  void import(const Board &board);
  void setRng(uint32_t seed);

  // One logic tick, Board::tick().
  void tick();

  // Board input, with the same preconditions as the real board.
  void moveCursor(Direction d);
  bool swapAtCursor();
  void forceRaise();
  // An action as AIBoardController::doAction() applies it.
  void apply(AIStrategyAction action);
  // apply() then tick(): one controller tick followed by one board tick.
  void step(AIStrategyAction action) {
    apply(action);
    tick();
  }

  // Board::swappable() for both halves plus "at least one is a block".
  bool canSwap(int x, int y) const;

  // Garbage the opponent sent (VsGame::handleGarbageSpawning()).
  void queueGarbage(bool fullWidth, int size, GarbageBlockType type);

  const Cell &cell(int row, int col) const {
    return _cells[row][col];
  }
  const Garbage &garbage(int i) const {
    return _garbage[i];
  }
  int numGarbage() const {
    return _numGarbage;
  }
  int numSpawns() const {
    return _numSpawns;
  }
  const Spawn &spawn(int i) const {
    return _spawns[i];
  }
  uint8_t bufferColor(int col) const {
    return _bufferColor[col];
  }

  Board::BoardState state() const {
    return _state;
  }
  bool dead() const {
    return _state == Board::GAME_OVER;
  }
  bool running() const {
    return _state == Board::RUNNING;
  }
  // Board::hasActiveBlocks(): the value computed at the start of the last
  // tick, which is also what gates a forced raise between ticks.
  bool activeFlag() const {
    return _active;
  }
  // Recomputed now: anything exploding, swapping, floating or falling.
  bool anyActive() const;
  int cursorX() const {
    return _cursorX;
  }
  int cursorY() const {
    return _cursorY;
  }
  int score() const {
    return _score;
  }
  int chainCounter() const {
    return _chainCounter;
  }
  int lastChain() const {
    return _lastChain;
  }
  int stackOffset() const {
    return _stackOffset;
  }
  int stackRaiseTicks() const {
    return _raiseTicks;
  }
  int stackRaiseTimer() const {
    return _raiseTimer;
  }
  bool raiseForced() const {
    return _forced;
  }
  int graceTimer() const {
    return _graceTimer;
  }
  bool panic() const {
    return _panic;
  }
  int tickMatched() const {
    return _tickMatched;
  }
  bool tickChain() const {
    return _tickChain;
  }
  bool tickChainEnd() const {
    return _tickChainEnd;
  }
  unsigned ticksRun() const {
    return _ticksRun;
  }
  // Ticks simulated since import().
  int simTicks() const {
    return _simTicks;
  }
  // A colour the model made up (a new buffer row, a garbage interior) is
  // on the board: predictions past this point are guesses.
  bool inventedColors() const {
    return _invented;
  }
  int spawnPosition(int i) const {
    return _spawnPos[i];
  }
  // Highest row holding anything (block, garbage, chain flag or swap half).
  int top() const {
    return _top;
  }
  // Rows the stack came up since import(): row numbers of everything on
  // the board grew by this much.
  int shifts() const {
    return _shifts;
  }

  const Stats &stats() const {
    return _stats;
  }
  void resetStats();

 private:
  Cell _cells[H][W];
  Garbage _garbage[MAX_GARBAGE];
  int _numGarbage;
  Spawn _spawns[MAX_SPAWNS];
  int _numSpawns;
  uint8_t _bufferColor[W];

  Board::BoardState _state;
  unsigned _ticksRun;
  int _simTicks;
  int _cursorX, _cursorY;
  int _tickMatched;
  bool _tickChain, _tickChainEnd;
  int _stackOffset;
  int _raiseTicks;
  int _raiseTimer;
  bool _forced;
  int _graceTimer;
  bool _active;
  int _chainCounter;
  int _lastChain;
  bool _blockOnTopRow;
  bool _panic;
  int _score;
  int _spawnPos[3];

  int _top;
  int _shifts;
  bool _changed;  // a cell changed since the last full pass (see tick())
  bool _invented;
  bool _bufferInvented;
  uint32_t _rng;
  Stats _stats;

  uint32_t rnd();
  uint8_t randomColor(int excludeA, int excludeB);
  void fillBufferRow();
  void fillGarbageColors(Garbage &g);
  void updateTop();

  static void clearCell(Cell &c);
  bool blockOnRow(int row) const;
  bool swappable(int row, int col) const;
  bool blockCanFall(int row, int col) const;
  bool garbageCanFall(const Garbage &g) const;
  bool matchTiles(int r1, int c1, int r2, int c2) const;
  bool checkChain(int row, int col) const;
  bool allAirAbove(int row, int col) const;

  void raiseStack();
  void handleGarbageQueue();
  bool spawnGarbage(int x, int y, int w, int h, uint8_t type);
  void removeGarbage(int idx);
  void handleBlockTimers();
  void swapBlocks(int row, int col);
  void handleFalling();
  void handleGarbageFalling();
  void matchBlocks();
  void handleMatchedBlocks();
  void triggerTile(int row, int col);
  void triggerGarbageNeighbors(int idx);
  void handleTriggeredBlocks();
  void sendEvents();
};

#endif /* AI_PROSIM_H_ */
