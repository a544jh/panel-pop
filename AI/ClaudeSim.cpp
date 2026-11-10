/*
 * ClaudeSim.cpp
 *
 * Each function below is named after, and follows, its counterpart in
 * Game/Board.cpp. Comments call out where the real board does something
 * surprising that is mirrored on purpose.
 */

#include "ClaudeSim.h"

#include <cstring>
#include <vector>

#include "../Game/Block.h"
#include "../Game/GarbageBlock.h"

namespace {

const int EXPL_BASE = Board::BASE_EXPLOSION_TICKS;
const int EXPL_ADD = Board::ADD_EXPL_TICKS;
const int RAISE_STEPS = Board::STACK_RAISE_STEPS;

// Board::chainScoring()
int chainScore(int chain) {
  switch (chain) {
    case 1: return 0;
    case 2: return 50;
    case 3: return 80;
    case 4: return 150;
    case 5: return 300;
    default: return (chain + 2) * 200;
  }
}

// Board::comboScoring(); note the negative values for big combos outside a
// chain, which the real board really does award.
int comboScore(int matched, int chain) {
  if (matched == 4) {
    return 20;
  }
  if (matched == 5) {
    return 30;
  }
  if (matched >= 6) {
    return (chain - 4) * 20;
  }
  return 0;
}

// Garbage cells VsGame::handleGarbageSpawning() sends for a combo.
int comboCells(int combo) {
  if (combo >= 4 && combo <= 7) {
    return combo - 1;
  }
  if (combo == 8) {
    return 3 + 4;
  }
  if (combo >= 9 && combo <= 10) {
    return 2 * (combo - 5);
  }
  if (combo == 11) {
    return 5 + 6;
  }
  return 0; // >= 12: the real loop never runs (5 - combo < 0)
}

} // namespace

ClaudeSim::ClaudeSim() {
  memset(this, 0, sizeof(*this));
  for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
      clearCell(_cells[r][c]);
    }
  }
  _state = Board::COUNTDOWN;
  _rng = 0x2545f491u;
  _stats.lastLinkTick = -1;
}

void ClaudeSim::clearCell(Cell &c) {
  c.type = AIR;
  c.color = BlockColor::COUNT;
  c.state = NORMAL;
  c.chain = 0;
  c.falling = 0;
  c.gbFall = 0;
  c.g = -1;
  c.swapTimer = 0;
  c.floatTimer = 0;
  c.explTimer = 0;
  c.explTicks = 0;
  c.animTicks = 0;
}

void ClaudeSim::resetStats() {
  memset(&_stats, 0, sizeof(_stats));
  _stats.maxChain = _chainCounter;
  _stats.lastLinkTick = -1;
  _simTicks = 0;
}

void ClaudeSim::setRng(uint32_t seed) {
  _rng = seed ? seed : 0x2545f491u;
}

uint32_t ClaudeSim::rnd() {
  // xorshift32
  uint32_t x = _rng;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  _rng = x;
  return x;
}

uint8_t ClaudeSim::randomColor(int excludeA, int excludeB) {
  uint8_t allowed[BlockColor::COUNT];
  int n = 0;
  for (int k = 0; k < BlockColor::COUNT; ++k) {
    if (k != excludeA && k != excludeB) {
      allowed[n++] = (uint8_t) k;
    }
  }
  return allowed[rnd() % n];
}

void ClaudeSim::fillBufferRow() {
  // Board::fillBufferRow(): not the colour above (row 0) nor the one to
  // the left.
  for (int i = 0; i < W; ++i) {
    int left = i > 0 ? _bufferColor[i - 1] : -1;
    _bufferColor[i] = randomColor(_cells[0][i].color, left);
  }
}

void ClaudeSim::fillGarbageColors(Garbage &g) {
  // GarbageBlock::fillBufferRow(): not the colour to the left.
  for (int i = 0; i < g.w; ++i) {
    int left = i > 0 ? g.color[i - 1] : -1;
    g.color[i] = randomColor(left, -1);
  }
}

void ClaudeSim::import(const Board &board) {
  _numGarbage = 0;
  std::vector<const GarbageBlock *> ptrs;
  for (const GarbageBlock &gb : board.getGarbageBlocks()) {
    if (_numGarbage >= MAX_GARBAGE) {
      break;
    }
    Garbage &g = _garbage[_numGarbage++];
    g.x = (int8_t) gb.getX();
    g.y = (int8_t) gb.getY();
    g.w = (int8_t) gb.getW();
    g.h = (int8_t) gb.getH();
    g.type = (uint8_t) gb.getType();
    g.state = (uint8_t) gb.getState();
    g.transTicks = (int16_t) gb.getTransformationTicks();
    g.transTimer = (int16_t) gb.getTransformationTimer();
    g.falling = gb.isFalling() ? 1 : 0;
    g.initialFall = gb.isInitialFall() ? 1 : 0;
    g.invented = 0;
    for (int i = 0; i < W; ++i) {
      g.color[i] = i < g.w ? (uint8_t) gb.getBufferRow(i)._color : (uint8_t) BlockColor::COUNT;
    }
    ptrs.push_back(&gb);
  }

  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      const Board::Tile &t = board.getTile(row, col);
      Cell &c = _cells[row][col];
      c.type = (uint8_t) t.type;
      c.color = (uint8_t) t.b._color;
      c.state = (uint8_t) t.b._state;
      c.chain = t.chain ? 1 : 0;
      c.falling = t.b._falling ? 1 : 0;
      c.gbFall = t.b._gbFallChain ? 1 : 0;
      c.swapTimer = (uint8_t) t.b._swapTimer;
      c.floatTimer = (int8_t) t.b._floatTimer;
      c.explTimer = (uint16_t) t.b._explosionTimer;
      c.explTicks = (uint16_t) t.b._explosionTicks;
      c.animTicks = (uint16_t) t.b._explosionAnimTicks;
      c.g = -1;
      if (t.type == TileType::GARBAGE) {
        for (size_t i = 0; i < ptrs.size(); ++i) {
          if (ptrs[i] == t.g) {
            c.g = (int8_t) i;
            break;
          }
        }
      }
    }
  }

  for (int i = 0; i < W; ++i) {
    _bufferColor[i] = (uint8_t) board.getBufferRow(i).b._color;
  }

  _numSpawns = 0;
  for (const Board::GarbageSpawn &s : board.getGarbageQueue()) {
    if (_numSpawns >= MAX_SPAWNS) {
      break;
    }
    Spawn &sp = _spawns[_numSpawns++];
    sp.fullWidth = s.fullWidth ? 1 : 0;
    sp.type = (uint8_t) s.type;
    sp.size = (int8_t) s.size;
    sp.timer = (int16_t) s.spawnTimer;
  }

  _state = board.getState();
  _ticksRun = board.getTicksRun();
  _cursorX = board.getCursorX();
  _cursorY = board.getCursorY();
  _tickMatched = board.getTickMatched();
  _tickChain = board.isTickChain();
  _tickChainEnd = board.isTickChainEnd();
  _stackOffset = board.getStackOffset();
  _raiseTicks = board.getStackRaiseTicks();
  _raiseTimer = board.getStackRaiseTimer();
  _forced = board.isStackRaiseForced();
  _graceTimer = board.getGraceTimer();
  _active = board.hasActiveBlocks();
  _chainCounter = board.getChainCounter();
  _lastChain = board.getLastChain();
  _panic = board.isPanic();
  _blockOnTopRow = false; // recomputed at the start of every tick
  _score = board.getScore();
  for (int i = 0; i < 3; ++i) {
    _spawnPos[i] = board.getGarbageSpawnPosition(i);
  }
  _changed = true;
  _invented = false;
  _bufferInvented = false;
  _shifts = 0;
  _top = H - 1;
  updateTop();
  resetStats();
}

void ClaudeSim::queueGarbage(bool fullWidth, int size, GarbageBlockType type) {
  if (_numSpawns >= MAX_SPAWNS) {
    return;
  }
  Spawn &sp = _spawns[_numSpawns++];
  sp.fullWidth = fullWidth ? 1 : 0;
  sp.type = (uint8_t) type;
  sp.size = (int8_t) size;
  sp.timer = 80;
}

void ClaudeSim::updateTop() {
  int row = _top < H - 1 ? _top + 1 : H - 1;
  for (; row >= 0; --row) {
    for (int col = 0; col < W; ++col) {
      const Cell &c = _cells[row][col];
      if (c.type != AIR || c.chain || c.state != NORMAL) {
        _top = row;
        return;
      }
    }
  }
  _top = -1;
}

// ---------------------------------------------------------------------------
// predicates
// ---------------------------------------------------------------------------

bool ClaudeSim::blockOnRow(int row) const {
  for (int col = 0; col < W; ++col) {
    const Cell &c = _cells[row][col];
    if (c.type != AIR && !(c.type == GARBAGE && c.g >= 0 && _garbage[c.g].initialFall)) {
      return true;
    }
  }
  return false;
}

bool ClaudeSim::anyActive() const {
  for (int row = 0; row <= _top; ++row) {
    for (int col = 0; col < W; ++col) {
      const Cell &c = _cells[row][col];
      if (c.type == BLOCK && (c.state != NORMAL || c.falling)) {
        return true;
      }
    }
  }
  for (int i = 0; i < _numGarbage; ++i) {
    if (_garbage[i].state != (uint8_t) GarbageBlockState::NORMAL || _garbage[i].falling) {
      return true;
    }
  }
  return false;
}

bool ClaudeSim::swappable(int row, int col) const {
  const Cell &t = _cells[row][col];
  return ((t.type == AIR && _cells[row + 1][col].type != BLOCK) || t.type == BLOCK)
      && t.state == NORMAL;
}

bool ClaudeSim::canSwap(int x, int y) const {
  if (x < 0 || x + 1 >= W || y < 0 || y > Board::TOP_ROW) {
    return false;
  }
  return swappable(y, x) && swappable(y, x + 1)
      && (_cells[y][x].type == BLOCK || _cells[y][x + 1].type == BLOCK);
}

bool ClaudeSim::blockCanFall(int row, int col) const {
  const Cell &c = _cells[row][col];
  if (c.type != BLOCK || row == 0 || c.state != NORMAL) {
    return false;
  }
  const Cell &b = _cells[row - 1][col];
  return b.type == AIR && b.state == NORMAL;
}

bool ClaudeSim::garbageCanFall(const Garbage &g) const {
  if (g.state != (uint8_t) GarbageBlockState::NORMAL) {
    return false;
  }
  int row = g.y - g.h;
  if (row < 0) {
    // The real board reads _tiles[-1] here (see AGENTS.md); treat the floor
    // as solid, which is what it is meant to be.
    return false;
  }
  for (int col = g.x; col < g.x + g.w; ++col) {
    const Cell &c = _cells[row][col];
    if (c.type != AIR || c.state != NORMAL) {
      return false;
    }
  }
  return true;
}

bool ClaudeSim::matchTiles(int r1, int c1, int r2, int c2) const {
  const Cell &a = _cells[r1][c1];
  const Cell &b = _cells[r2][c2];
  return a.type == BLOCK && b.type == BLOCK
      && (a.state == NORMAL || a.state == MATCHED)
      && (b.state == NORMAL || b.state == MATCHED)
      && !a.falling && !b.falling && a.color == b.color;
}

bool ClaudeSim::checkChain(int row, int col) const {
  for (int i = row; i >= 0; --i) {
    if (_cells[i][col].chain) {
      return true;
    }
  }
  return false;
}

bool ClaudeSim::allAirAbove(int row, int col) const {
  for (int i = row; i <= _top; ++i) {
    const Cell &t = _cells[i][col];
    if (t.type == AIR || t.type == GARBAGE
        || (t.type == BLOCK && t.state == NORMAL && !t.falling)) {
      continue;
    }
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// input
// ---------------------------------------------------------------------------

void ClaudeSim::moveCursor(Direction d) {
  if (_state != Board::RUNNING && _state != Board::COUNTDOWN) {
    return;
  }
  switch (d) {
    case UP:
      if (_cursorY + 1 < 12) {
        ++_cursorY;
      }
      break;
    case DOWN:
      if (_cursorY - 1 >= 0) {
        --_cursorY;
      }
      break;
    case RIGHT:
      if (_cursorX + 1 < W - 1) {
        ++_cursorX;
      }
      break;
    case LEFT:
      if (_cursorX - 1 >= 0) {
        --_cursorX;
      }
      break;
    default:
      break;
  }
}

bool ClaudeSim::swapAtCursor() {
  if (_state != Board::RUNNING) {
    return false;
  }
  if (!canSwap(_cursorX, _cursorY)) {
    return false;
  }
  _cells[_cursorY][_cursorX].state = SWAPPING_RIGHT;
  _cells[_cursorY][_cursorX + 1].state = SWAPPING_LEFT;
  if (_cursorY > _top) {
    _top = _cursorY;
  }
  _changed = true;
  return true;
}

void ClaudeSim::forceRaise() {
  if (_state != Board::RUNNING) {
    return;
  }
  if (!_active) {
    _forced = true;
  }
}

void ClaudeSim::apply(AIStrategyAction action) {
  switch (action) {
    case AI_UP: moveCursor(UP);
      break;
    case AI_DOWN: moveCursor(DOWN);
      break;
    case AI_LEFT: moveCursor(LEFT);
      break;
    case AI_RIGHT: moveCursor(RIGHT);
      break;
    case AI_SWAP: swapAtCursor();
      break;
    case AI_RAISE: forceRaise();
      break;
    case AI_WAIT:
      break;
  }
}

// ---------------------------------------------------------------------------
// Board::tick()
// ---------------------------------------------------------------------------

void ClaudeSim::tick() {
  if (_state != Board::RUNNING) {
    // The countdown is driven by the game (advanceCountdownState()), which
    // the model does not see; nothing else ticks outside RUNNING.
    ++_ticksRun;
    ++_simTicks;
    return;
  }

  // initTick()
  _tickMatched = 0;
  _tickChain = false;
  _tickChainEnd = false;
  _active = anyActive();
  if (!_active) {
    if (_chainCounter > 1) {
      _tickChainEnd = true;
      _lastChain = _chainCounter;
      int rows = _chainCounter - 1;
      _stats.chainRows += rows > 12 ? 12 : rows;
      if (_stats.numChains < 8) {
        _stats.chainLen[_stats.numChains++] = _chainCounter;
      }
    }
    _chainCounter = 1;
  }
  _blockOnTopRow = blockOnRow(Board::TOP_ROW);
  // Board::initTick() overwrites the flag on every loop iteration, so it
  // ends up meaning "something on row PANIC_HEIGHT".
  _panic = blockOnRow(Board::PANIC_HEIGHT);
  if (_ticksRun % 1440 == 1439 && _raiseTicks > 0) {
    --_raiseTicks;
  }

  raiseStack();
  handleGarbageQueue();
  handleBlockTimers();

  // With nothing active and nothing changed since the last full pass, the
  // remaining steps cannot do anything: nothing can fall or match, and the
  // chain flags were already cleaned up by that pass.
  if (_active || _changed) {
    _changed = false;
    handleFalling();
    handleGarbageFalling();
    matchBlocks();
    handleMatchedBlocks();
    handleTriggeredBlocks();
  }
  sendEvents();

  if (_chainCounter > _stats.maxChain) {
    _stats.maxChain = _chainCounter;
  }
  updateTop();
  ++_ticksRun;
  ++_simTicks;
}

void ClaudeSim::raiseStack() {
  if (!_forced && _raiseTimer < _raiseTicks) {
    ++_raiseTimer;
    return;
  }
  if (_stackOffset < RAISE_STEPS && !_active && !_blockOnTopRow) {
    ++_stackOffset;
    _raiseTimer = 0;
  }
  if (_blockOnTopRow && !_active) {
    if (++_graceTimer >= (RAISE_STEPS * _raiseTicks) / 2) {
      _state = Board::GAME_OVER;
    }
  }
  if (!_blockOnTopRow) {
    _graceTimer = 0;
  }
  if (_stackOffset >= RAISE_STEPS && !_blockOnTopRow && !_active) {
    for (int i = 0; i < _numGarbage; ++i) {
      ++_garbage[i].y;
    }
    for (int row = H - 2; row >= 0; --row) {
      memcpy(_cells[row + 1], _cells[row], sizeof(_cells[row]));
    }
    for (int i = 0; i < W; ++i) {
      Cell &c = _cells[0][i];
      clearCell(c);
      c.type = BLOCK;
      c.color = _bufferColor[i];
    }
    // The buffer row read by import() is the real one; every row after it
    // is made up by the model.
    if (_bufferInvented) {
      _invented = true;
    }
    fillBufferRow();
    _bufferInvented = true;
    _stackOffset = 0;
    if (_forced) {
      ++_score;
    }
    _forced = false;
    if (_cursorY <= 10) {
      ++_cursorY;
    }
    if (_top < H - 1) {
      ++_top;
    }
    ++_shifts;
    _changed = true;
  }
}

bool ClaudeSim::spawnGarbage(int x, int y, int w, int h, uint8_t type) {
  if (y >= H || x >= W) {
    return false;
  }
  for (int row = y - h; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      if (_cells[row][col].type != AIR) {
        return false;
      }
    }
  }
  if (_numGarbage >= MAX_GARBAGE) {
    return false;
  }
  Garbage &g = _garbage[_numGarbage];
  g.x = (int8_t) x;
  g.y = (int8_t) y;
  g.w = (int8_t) w;
  g.h = (int8_t) h;
  g.type = type;
  g.state = (uint8_t) GarbageBlockState::NORMAL;
  g.transTicks = 0;
  g.transTimer = 0;
  g.falling = 0;
  g.initialFall = 1;
  g.invented = 1;
  for (int i = 0; i < W; ++i) {
    g.color[i] = (uint8_t) BlockColor::COUNT;
  }
  fillGarbageColors(g);
  for (int row = y; row >= y - (h - 1); --row) {
    for (int col = x; col <= x + (w - 1); ++col) {
      _cells[row][col].type = GARBAGE;
      _cells[row][col].g = (int8_t) _numGarbage;
    }
  }
  ++_numGarbage;
  if (y > _top) {
    _top = y;
  }
  _changed = true;
  return true;
}

void ClaudeSim::handleGarbageQueue() {
  if (_numSpawns == 0) {
    return;
  }
  int y = Board::TOP_ROW;
  for (int row = H - 1; row >= y; --row) {
    bool any = false;
    for (int col = 0; col < W; ++col) {
      if (_cells[row][col].type != AIR) {
        any = true;
        break;
      }
    }
    if (any) {
      y = row + 1;
      break;
    }
  }

  // Board iterates a std::list and does "it = erase(it); continue;", so
  // the spawn after an erased one is skipped this tick, and erasing the last
  // one wraps the iterator around to begin() (libstdc++'s circular list)
  // and goes through the remaining spawns a second time.
  int i = 0;
  while (i < _numSpawns) {
    if (y >= H) {
      break;
    }
    Spawn &s = _spawns[i];
    if (--s.timer <= 0 && !_active) {
      int x = 0;
      int w, h;
      if (s.fullWidth) {
        w = W;
        h = s.size;
        y += h - 1;
      } else {
        if (s.size == 3) {
          x = _spawnPos[0]++ % 2 * 3;
        } else if (s.size < 6) {
          x = _spawnPos[s.size - 3]++ % (6 - s.size + 1);
        }
        w = s.size;
        h = 1;
      }
      if (spawnGarbage(x, y, w, h, s.type)) {
        ++y;
        for (int k = i; k + 1 < _numSpawns; ++k) {
          _spawns[k] = _spawns[k + 1];
        }
        --_numSpawns;
        if (i >= _numSpawns) {
          i = 0;   // erased the last one: ++end() == begin()
        } else {
          ++i;     // the one that moved into slot i is skipped
        }
        continue;
      }
    }
    ++i;
  }
}

void ClaudeSim::removeGarbage(int idx) {
  for (int k = idx; k + 1 < _numGarbage; ++k) {
    _garbage[k] = _garbage[k + 1];
  }
  --_numGarbage;
  for (int row = 0; row <= _top; ++row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == GARBAGE && c.g > idx) {
        --c.g;
      }
    }
  }
}

void ClaudeSim::swapBlocks(int row, int col) {
  Cell &right = _cells[row][col];      // the SWAPPING_RIGHT half
  Cell &left = _cells[row][col + 1];   // the SWAPPING_LEFT half
  if (left.state == SWAPPING_LEFT) {
    uint8_t rChain = right.chain;
    uint8_t lChain = left.chain;
    Cell tmp = left;
    left = right;
    right = tmp;
    right.chain = rChain;
    left.chain = lChain;
  }
  right.swapTimer = 0;
  right.state = NORMAL;
  left.swapTimer = 0;
  left.state = NORMAL;
}

void ClaudeSim::handleBlockTimers() {
  for (int row = 0; row <= _top; ++row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == BLOCK && c.state == EXPLODING) {
        ++c.explTimer;
        if (c.explTimer == c.animTicks) {
          _score += 10;
        }
        if (c.explTicks == c.explTimer) {
          clearCell(c);
          c.chain = 1;
          ++_stats.blocksCleared;
          _changed = true;
        }
      }
      if (c.state == SWAPPING_LEFT || c.state == SWAPPING_RIGHT) {
        ++c.swapTimer;
        if (c.swapTimer >= Board::SWAP_DELAY && c.state == SWAPPING_RIGHT) {
          swapBlocks(row, col);
          _changed = true;
        }
      }
    }
  }

  // Same std::list erase-and-continue pattern as handleGarbageQueue().
  int i = 0;
  while (i < _numGarbage) {
    Garbage &g = _garbage[i];
    if (g.state == (uint8_t) GarbageBlockState::TRANSFORMING) {
      ++g.transTimer;
      if (g.transTimer == g.transTicks) {
        g.transTicks = 0;
        g.transTimer = 0;
        g.state = (uint8_t) GarbageBlockState::NORMAL;
        const int row = g.y - (g.h - 1);
        for (int k = 0; k < g.w; ++k) {
          Cell &c = _cells[row][g.x + k];
          uint8_t chain = c.chain; // Board only overwrites type, g and b
          clearCell(c);
          c.chain = chain;
          c.type = BLOCK;
          c.color = g.color[k];
          c.gbFall = 1;
        }
        if (g.invented) {
          _invented = true;
        }
        _stats.garbageCleared += g.w;
        _changed = true;
        if (--g.h <= 0) {
          removeGarbage(i);
          if (i >= _numGarbage) {
            i = 0;
          } else {
            ++i;
          }
          continue;
        }
        fillGarbageColors(g);
        g.invented = 1;
      }
    }
    ++i;
  }
}

void ClaudeSim::handleFalling() {
  for (int col = 0; col < W; ++col) {
    for (int row = 0; row <= _top; ++row) {
      Cell &c = _cells[row][col];
      if (c.state == FLOATING) {
        if (--c.floatTimer <= 0) {
          c.state = NORMAL;
          c.falling = 1;
          for (int y = row; y <= _top; ++y) {
            if (_cells[y][col].type == BLOCK) {
              _cells[y][col].falling = 1;
            }
          }
          _changed = true;
        }
      }
      if (blockCanFall(row, col)) {
        if (!c.falling) {
          c.state = FLOATING;
          c.floatTimer = Board::FLOAT_TICKS;
        } else {
          Cell &below = _cells[row - 1][col];
          uint8_t belowChain = below.chain;
          uint8_t chain = c.chain;
          below = c;
          below.chain = belowChain;
          clearCell(c);
          c.chain = chain;
        }
        _changed = true;
      } else if (c.falling) {
        c.falling = 0;
        _changed = true;
      }
    }
  }
}

void ClaudeSim::handleGarbageFalling() {
  for (int i = 0; i < _numGarbage; ++i) {
    Garbage &g = _garbage[i];
    if (garbageCanFall(g)) {
      g.falling = 1;
      const int row = g.y - g.h;
      for (int col = g.x; col < g.x + g.w; ++col) {
        clearCell(_cells[g.y][col]);
        if (g.y < H - 1 && _cells[g.y + 1][col].type == BLOCK) {
          _cells[g.y + 1][col].falling = 1;
        }
      }
      for (int col = g.x; col < g.x + g.w; ++col) {
        _cells[row][col].type = GARBAGE;
        _cells[row][col].g = (int8_t) i;
      }
      --g.y;
      _changed = true;
    } else if (g.falling) {
      g.initialFall = 0;
      g.falling = 0;
      _changed = true;
    }
  }
}

void ClaudeSim::matchBlocks() {
  for (int row = 0; row <= _top; ++row) {
    int matched = 1;
    int start = 0;
    for (int col = 1; col <= W; ++col) {
      if (col < W && matchTiles(row, col, row, col - 1)) {
        ++matched;
        continue;
      }
      if (matched >= 3) {
        for (int i = start; i < start + matched; ++i) {
          _cells[row][i].state = MATCHED;
          ++_tickMatched;
        }
        _changed = true;
      }
      start = col;
      matched = 1;
    }
  }
  for (int col = 0; col < W; ++col) {
    int matched = 1;
    int start = 0;
    for (int row = 1; row <= _top + 1; ++row) {
      if (row <= _top && matchTiles(row, col, row - 1, col)) {
        ++matched;
        continue;
      }
      if (matched >= 3) {
        for (int i = start; i < start + matched; ++i) {
          _cells[i][col].state = MATCHED;
          ++_tickMatched;
        }
        _changed = true;
      }
      start = row;
      matched = 1;
    }
  }
}

void ClaudeSim::handleMatchedBlocks() {
  int matches = 1;
  for (int row = _top; row >= 0; --row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == BLOCK && c.state == MATCHED) {
        if (checkChain(row, col) || c.gbFall) {
          _tickChain = true;
        }
        c.state = EXPLODING;
        c.explTimer = 0;
        c.animTicks = (uint16_t) (EXPL_BASE + matches * EXPL_ADD);
        c.explTicks = (uint16_t) (EXPL_BASE + _tickMatched * EXPL_ADD);
        triggerTile(row + 1, col);
        triggerTile(row, col + 1);
        triggerTile(row - 1, col);
        triggerTile(row, col - 1);
        ++matches;
      }
      if ((c.type == BLOCK && c.state == NORMAL && !c.falling)
          || (c.type == AIR && allAirAbove(row, col))) {
        c.chain = 0;
        c.gbFall = 0;
      }
    }
  }
  if (_tickChain) {
    ++_chainCounter;
    _score += chainScore(_chainCounter);
    ++_stats.links;
    _stats.lastLinkTick = _simTicks;
  }
}

void ClaudeSim::triggerTile(int row, int col) {
  if (row < 0 || col < 0 || row >= H || col >= W) {
    return;
  }
  const Cell &t = _cells[row][col];
  // The triggerer passed along by Board is always the matched block, so the
  // garbage type check there never refuses: any touching garbage goes.
  if (t.type == GARBAGE && t.g >= 0
      && _garbage[t.g].state == (uint8_t) GarbageBlockState::NORMAL) {
    _garbage[t.g].state = (uint8_t) GarbageBlockState::TRIGGERED;
    triggerGarbageNeighbors(t.g);
  }
}

void ClaudeSim::triggerGarbageNeighbors(int idx) {
  const Garbage g = _garbage[idx];
  for (int col = g.x; col <= g.x + (g.w - 1); ++col) {
    triggerTile(g.y + 1, col);
    triggerTile(g.y - g.h, col);
  }
  for (int row = g.y; row >= g.y - (g.h - 1); --row) {
    triggerTile(row, g.x - 1);
    triggerTile(row, g.x + g.w);
  }
}

void ClaudeSim::handleTriggeredBlocks() {
  int animStart = EXPL_BASE + (_tickMatched + 1) * EXPL_ADD;
  bool any = false;
  for (int row = 0; row <= _top; ++row) {
    for (int col = W - 1; col >= 0; --col) {
      const Cell &t = _cells[row][col];
      // g < 0 only if the real board had more garbage blocks than the model
      // keeps (MAX_GARBAGE): leave those alone rather than read past the end.
      if (t.type != GARBAGE || t.g < 0) {
        continue;
      }
      Garbage &g = _garbage[t.g];
      if (g.state == (uint8_t) GarbageBlockState::TRIGGERED && g.transTimer != 1) {
        g.transTimer = 1;
        animStart += g.w * g.h * EXPL_ADD;
        any = true;
      }
    }
  }
  if (!any) {
    return;
  }
  for (int i = 0; i < _numGarbage; ++i) {
    Garbage &g = _garbage[i];
    if (g.state == (uint8_t) GarbageBlockState::TRIGGERED && g.transTimer == 1) {
      g.transTimer = 0;
      g.state = (uint8_t) GarbageBlockState::TRANSFORMING;
      g.transTicks = (int16_t) animStart;
      _changed = true;
    }
  }
}

void ClaudeSim::sendEvents() {
  if (_tickMatched > 0) {
    ++_stats.matches;
  }
  if (_tickMatched > 3) {
    _score += chainScore(_chainCounter);
    _score += comboScore(_tickMatched, _chainCounter);
    _stats.comboCells += comboCells(_tickMatched);
    ++_stats.combos;
  }
}
