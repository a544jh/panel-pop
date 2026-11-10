/*
 * SimBoard.h
 *
 * Compact, copyable model of a game board used by QwenStrat to look ahead at
 * what a swap would do. It mirrors Board::tick() - stack advance, swaps,
 * gravity, matching, chain flags, garbage triggering/transformation and the
 * game over condition - with plain arrays instead of the pointer based Tile
 * struct, and without the rendering events. Timings and scoring follow the
 * real board; a few cosmetic details (explosion animation order, sound
 * effects) are left out.
 *
 * Not exact: new buffer row and garbage colours are drawn from the model's
 * own PRNG (see setRng()), because the real board fills them with rand().
 */

#ifndef AI_SIMBOARD_H_
#define AI_SIMBOARD_H_

#include <cstdint>

#include "../Game/Block.h"
#include "../Game/Board.h"
#include "../Game/GarbageBlock.h"

class SimBoard {
 public:
  static const int H = Board::BOARD_HEIGHT;
  static const int W = Board::BOARD_WIDTH;
  static const int MAX_GARBAGE = 8;

  enum CellType {
    AIR = 0, BLOCK = 1, GARBAGE = 2
  };

  struct Cell {
    uint8_t type; // CellType
    uint8_t color; // BlockColor for blocks
    int8_t g; // index into garbage[] for GARBAGE cells, else -1
    uint8_t state; // BlockState for blocks
    uint8_t swapTimer; // counts up while SWAPPING_LEFT/RIGHT
    int8_t floatTimer; // counts down while FLOATING
    uint16_t explTimer; // counts up while EXPLODING
    uint16_t explTicks; // explTimer value at which the cell clears
    uint8_t chain; // "chain zone" flag of the position
    uint8_t falling; // block is mid-air
    uint8_t gbFall; // block came out of transformed garbage
  };

  struct Garbage {
    int16_t x, y, w, h; // (x, y) top row / left column, as in GarbageBlock
    uint8_t type; // 0 NORMAL, 1 GRAY
    uint8_t state; // 0 NORMAL, 1 TRIGGERED, 2 TRANSFORMING
    int16_t transTicks, transTimer, animStart;
    uint8_t falling;
    uint8_t color[6]; // colours revealed one row at a time (buffer row)
  };

  struct Spawn { // pending garbage, mirrors Board::GarbageSpawn
    bool fullWidth;
    GarbageBlockType type;
    int size;
    int spawnTimer;
  };

  SimBoard();

  // Copies everything relevant from a live board.
  void import(const Board &board);

  // Deterministic PRNG for the new colours the model has to invent (buffer
  // rows, garbage interiors). Resetting it before each candidate simulation
  // makes the candidates comparable.
  void setRng(uint32_t seed);

  void tick(); // one logic tick, mirrors Board::tick()

  // Mirrors the preconditions of Board::inputSwapBlocks(): the cursor picks
  // the pair (x, y) and (x + 1, y).
  bool canSwap(int x, int y) const;
  // Starts the swap; the tiles actually exchange places SWAP_DELAY ticks
  // later, exactly as on the real board.
  void startSwap(int x, int y);
  // Forces the next stack step, as Board::inputForceStackRaise() does.
  void forceRaise();

  const Cell &cell(int row, int col) const {
    return _cells[row][col];
  }
  Cell &cell(int row, int col) {
    return _cells[row][col];
  }
  // Highest occupied row (see updateTopRow()).
  int topRow() const {
    return _topRow;
  }
  int numGarbage() const {
    return _numGarbage;
  }
  const Garbage &garbage(int i) const {
    return _garbage[i];
  }
  const std::vector<Spawn> &spawns() const {
    return _spawns;
  }

  Board::BoardState state() const {
    return _state;
  }
  bool isDead() const {
    return _state == Board::GAME_OVER;
  }
  bool hasActiveBlocks() const {
    return _activeBlocks;
  }
  int score() const {
    return _score;
  }
  int stackOffset() const {
    return _stackOffset;
  }
  int stackRaiseTicks() const {
    return _stackRaiseTicks;
  }
  int stackRaiseTimer() const {
    return _stackRaiseTimer;
  }
  int chainCounter() const {
    return _chainCounter;
  }
  int graceTimer() const {
    return _graceTimer;
  }
  int tickMatched() const {
    return _tickMatched;
  }
  int lastChain() const {
    return _lastChain;
  }
  int maxChain() const {
    return _maxChain;
  }
  // Rows of garbage this board would have sent to its opponent.
  int garbageSent() const {
    return _garbageSent;
  }
  int highestOccupiedRow() const {
    for (int row = _topRow; row >= 0; --row) {
      for (int col = 0; col < W; ++col) {
        if (_cells[row][col].type != AIR) {
          return row;
        }
      }
    }
    return -1;
  }
  int blocksAbove(int row) const {
    int n = 0;
    for (int r = row; r <= _topRow; ++r) {
      for (int col = 0; col < W; ++col) {
        if (_cells[r][col].type == BLOCK) {
          ++n;
        }
      }
    }
    return n;
  }
  // Cells still waiting for their explosion to finish, and blocks that are
  // mid-swap: both are "value on the way" the search should not miss.
  int pendingExplosions() const {
    int n = 0;
    for (int r = 0; r <= _topRow; ++r) {
      for (int c = 0; c < W; ++c) {
        const Cell &cell = _cells[r][c];
        if (cell.type == BLOCK && (cell.state == EXPLODING || cell.state == MATCHED)) {
          ++n;
        }
      }
    }
    return n;
  }
  int spawnCounter(int i) const {
    return _spawnCounters[i];
  }
  // Total height of the garbage still on the board.
  int garbageRows() const {
    int n = 0;
    for (int i = 0; i < _numGarbage; ++i) {
      n += _garbage[i].h;
    }
    return n;
  }

  // Board::blockOnRow(), i.e. "something is resting on that row".
  bool blockOnRow(int row) const;

  // Rows of full width garbage a chain end would send to the opponent
  // (VsGame::handleGarbageSpawning()).
  int garbageForChain(int chain) const;

 private:
  Cell _cells[H][W];
  Garbage _garbage[MAX_GARBAGE];
  int _numGarbage;
  Cell _bufferRow[W];
  std::vector<Spawn> _spawns;

  Board::BoardState _state;
  int _score;
  int _stackOffset;
  int _stackRaiseTicks;
  int _stackRaiseTimer;
  bool _stackRaiseForced;
  int _graceTimer;
  // Highest row that holds anything; keeps the per-tick loops short.
  int _topRow;
  // Set when a cell changed since the last tick (swap, fall, match, spawn).
  // Without one there can be no new match, so matching can be skipped.
  bool _changed;
  int _chainCounter;
  int _lastChain;
  int _tickMatched;
  int _maxChain;
  int _garbageSent;
  int _spawnCounters[3];
  bool _activeBlocks;
  bool _blockOnTopRow;
  uint32_t _rng;

  uint32_t rnd();
  // Same colour choice as Board::fillBufferRow()/GarbageBlock::fillBufferRow():
  // a random colour different from the one to the left.
  BlockColor randomColor(BlockColor left);

  void fillBufferRow();
  void fillGarbageColors(Garbage &g);
  bool activeBlocks() const;
  void updateTopRow();
  bool garbageCanFall(const Garbage &g) const;
  void raiseStack();
  void handleGarbageQueue();
  void handleBlockTimers();
  void handleFalling();
  void handleGarbageFalling();
  void matchBlocks();
  void handleMatchedBlocks();
  void handleTriggeredBlocks();
  void triggerNeighbors(int row, int col);
  // triggerGarbageType is the garbage type of the block that started the
  // cascade (-1 when a normal block triggered it), mirroring the "t" tile
  // Board::triggerGarbageNeighbors() passes along.
  void triggerGarbageNeighbors(const Garbage &g, int triggerGarbageType);
  void triggerTile(int row, int col, int triggerGarbageType);
  bool checkChain(int row, int col) const;
  bool allAirAbove(int row, int col) const;
  bool matchTiles(int r1, int c1, int r2, int c2) const;
  int spawnGarbage(int x, int y, int w, int h, GarbageBlockType type);
  void removeGarbage(int idx);
  void swapBlocks(int row, int col);
  void clearCell(Cell &c);
};

#endif /* AI_SIMBOARD_H_ */
