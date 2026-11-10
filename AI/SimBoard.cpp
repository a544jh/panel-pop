/*
 * SimBoard.cpp
 */

#include "SimBoard.h"

#include <cstring>
#include <vector>

namespace {

// Board::Cell state helpers: the model stores the same enums as the real
// board, just packed into plain integer fields.
const int GS_NORMAL = 0;
const int GS_TRIGGERED = 1;
const int GS_TRANSFORMING = 2;

// Board::chainScoring() / Board::comboScoring(), for the model's score total.
int chainScore(int chainCounter) {
  switch (chainCounter) {
    case 1: return 0;
    case 2: return 50;
    case 3: return 80;
    case 4: return 150;
    case 5: return 300;
    default: return (chainCounter + 2) * 200;
  }
}

// Rows of garbage VsGame::handleGarbageSpawning() sends for a combo.
int comboGarbage(int combo) {
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
  // combo >= 12: the real loop runs "5 - combo" times, i.e. not at all
  return 0;
}

int comboScore(int matched, int chainCounter) {
  if (matched == 4) {
    return 20;
  }
  if (matched == 5) {
    return 30;
  }
  if (matched >= 6) {
    return (chainCounter - 4) * 20;
  }
  return 0;
}

} // namespace

SimBoard::SimBoard() :
    _numGarbage(0),
    _state(Board::RUNNING),
    _score(0),
    _stackOffset(0),
    _stackRaiseTicks(10),
    _stackRaiseTimer(0),
    _stackRaiseForced(false),
    _graceTimer(0),
    _topRow(H - 1),
    _changed(true),
    _chainCounter(1),
    _lastChain(0),
    _tickMatched(0),
    _maxChain(1),
    _garbageSent(0),
    _spawnCounters{0, 0, 0},
    _activeBlocks(false),
    _blockOnTopRow(false),
    _rng(0x12345678u) {
  for (int col = 0; col < W; ++col) {
    Cell &c = _bufferRow[col];
    c.type = AIR;
    c.color = BlockColor::COUNT;
    c.g = -1;
    c.state = NORMAL;
    c.chain = 0;
    c.falling = 0;
    c.gbFall = 0;
    c.swapTimer = 0;
    c.floatTimer = 0;
    c.explTimer = 0;
    c.explTicks = 0;
  }
  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      c.type = AIR;
      c.color = BlockColor::COUNT;
      c.g = -1;
      c.state = NORMAL;
      c.swapTimer = 0;
      c.floatTimer = 0;
      c.explTimer = 0;
      c.explTicks = 0;
      c.chain = 0;
      c.falling = 0;
      c.gbFall = 0;
    }
  }
  for (int i = 0; i < W; ++i) {
    _bufferRow[i] = _cells[0][0];
    _bufferRow[i].type = BLOCK;
  }
  for (int i = 0; i < MAX_GARBAGE; ++i) {
    std::memset(&_garbage[i], 0, sizeof(Garbage));
    _garbage[i].state = GS_NORMAL;
  }
}

void SimBoard::setRng(uint32_t seed) {
  _rng = seed ? seed : 1u;
}

uint32_t SimBoard::rnd() {
  // xorshift32
  _rng ^= _rng << 13;
  _rng ^= _rng >> 17;
  _rng ^= _rng << 5;
  return _rng;
}

BlockColor SimBoard::randomColor(BlockColor left) {
  // Same as Block::getRandomColor() over the list of all colours minus the
  // colour to the left (see Board::fillBufferRow).
  int n = BlockColor::COUNT;
  int idx = 0;
  if (left != BlockColor::COUNT) {
    n--;
    idx = rnd() % n;
    if (idx >= (int) left) {
      idx++;
    }
  } else {
    idx = rnd() % n;
  }
  return static_cast<BlockColor> (idx);
}

void SimBoard::fillBufferRow() {
  // Board::fillBufferRow(): every column gets a random colour different from
  // the block below it and from the buffer row cell to its left.
  BlockColor below[W];
  for (int i = 0; i < W; ++i) {
    below[i] = _cells[0][i].type == BLOCK ? static_cast<BlockColor> (_cells[0][i].color)
        : BlockColor::COUNT;
  }
  for (int i = 0; i < W; ++i) {
    _bufferRow[i].type = BLOCK;
    _bufferRow[i].state = NORMAL;
    _bufferRow[i].chain = 0;
    _bufferRow[i].falling = 0;
    _bufferRow[i].gbFall = 0;
    _bufferRow[i].explTimer = 0;
    _bufferRow[i].explTicks = 0;
    _bufferRow[i].g = -1;
    BlockColor left = i > 0 ? static_cast<BlockColor> (_bufferRow[i - 1].color) : BlockColor::COUNT;
    BlockColor color = randomColor(left);
    if (below[i] != BlockColor::COUNT && color == below[i]) {
      // The real board only excludes the block below for the *first* row it
      // fills; the buffer row may still repeat a colour vertically. Keeping
      // that quirk, nothing to do here.
    }
    _bufferRow[i].color = color;
  }
}

void SimBoard::fillGarbageColors(Garbage &g) {
  for (int i = 0; i < g.w && i < W; ++i) {
    BlockColor left = i > 0 ? static_cast<BlockColor> (g.color[i - 1]) : BlockColor::COUNT;
    g.color[i] = randomColor(left);
  }
}

void SimBoard::import(const Board &board) {
  _spawns.clear();
  _numGarbage = 0;

  std::vector<const GarbageBlock *> ptrs;
  for (const GarbageBlock &gb : board.getGarbageBlocks()) {
    if (_numGarbage >= MAX_GARBAGE) {
      break;
    }
    Garbage &g = _garbage[_numGarbage++];
    g.x = gb.getX();
    g.y = gb.getY();
    g.w = gb.getW();
    g.h = gb.getH();
    g.type = (gb.getType() == GarbageBlockType::GRAY) ? 1 : 0;
    g.state = gb.getState() == GarbageBlockState::NORMAL ? GS_NORMAL
        : (gb.getState() == GarbageBlockState::TRIGGERED ? GS_TRIGGERED : GS_TRANSFORMING);
    g.transTicks = gb.getTransformationTicks();
    g.transTimer = gb.getTransformationTimer();
    g.animStart = gb.getAnimationStart();
    g.falling = 0;
    for (int i = 0; i < g.w && i < W; ++i) {
      g.color[i] = gb.getBufferRow(i)._color;
    }
    ptrs.push_back(&gb);
  }

  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      const Board::Tile &t = board.getTile(row, col);
      const int ttype = t.type;
      Cell &c = _cells[row][col];
      c.chain = t.chain ? 1 : 0;
      c.falling = 0;
      c.gbFall = 0;
      c.swapTimer = 0;
      c.floatTimer = 0;
      c.explTimer = 0;
      c.explTicks = 0;
      c.g = -1;
      if (ttype == TileType::AIR) {
        c.type = AIR;
        c.color = BlockColor::COUNT;
        c.state = t.b._state;
        continue;
      }
      if (ttype == TileType::GARBAGE) {
        c.type = GARBAGE;
        c.color = BlockColor::COUNT;
        c.state = NORMAL;
        for (size_t i = 0; i < ptrs.size(); ++i) {
          if (ptrs[i] == t.g) {
            c.g = (int8_t) i;
            break;
          }
        }
        continue;
      }
      c.type = BLOCK;
      c.color = t.b._color;
      c.state = t.b._state;
      c.falling = t.b._falling ? 1 : 0;
      c.gbFall = t.b._gbFallChain ? 1 : 0;
      c.swapTimer = t.b._swapTimer;
      if (t.b._state == EXPLODING) {
        c.explTimer = t.b._explosionTimer;
        c.explTicks = t.b._explosionTicks;
      }
    }
  }

  for (int i = 0; i < W; ++i) {
    const Board::Tile &t = board.getBufferRow(i);
    _bufferRow[i] = _cells[0][0];
    _bufferRow[i].type = BLOCK;
    _bufferRow[i].state = NORMAL;
    _bufferRow[i].color = t.b._color;
  }

  for (const Board::GarbageSpawn &s : board.getGarbageQueue()) {
    Spawn spawn;
    spawn.fullWidth = s.fullWidth;
    spawn.type = s.type;
    spawn.size = s.size;
    spawn.spawnTimer = s.spawnTimer;
    _spawns.push_back(spawn);
  }

  _state = board.getState();
  _score = board.getScore();
  _stackOffset = board.getStackOffset();
  _stackRaiseTicks = board.getStackRaiseTicks();
  _stackRaiseTimer = board.getStackRaiseTimer();
  _stackRaiseForced = board.isStackRaiseForced();
  _graceTimer = board.getGraceTimer();
  _chainCounter = board.getChainCounter();
  _lastChain = board.getLastChain();
  _maxChain = board.getChainCounter();
  _garbageSent = 0;
  _activeBlocks = board.hasActiveBlocks();
  _blockOnTopRow = false;
  _tickMatched = board.getTickMatched();
  // import() has to be self-contained: start the scan from the top.
  _topRow = H - 1;
  updateTopRow();
  _changed = true;
}

bool SimBoard::blockOnRow(int row) const {
  for (int col = 0; col < W; ++col) {
    const Cell &c = _cells[row][col];
    if (c.type != AIR) {
      return true;
    }
  }
  return false;
}

bool SimBoard::activeBlocks() const {
  for (int row = 0; row <= _topRow; ++row) {
    for (int col = 0; col < W; ++col) {
      const Cell &c = _cells[row][col];
      if (c.type == BLOCK && (c.state != NORMAL || c.falling)) {
        return true;
      }
    }
  }
  for (int i = 0; i < _numGarbage; ++i) {
    if (_garbage[i].state != GS_NORMAL || _garbage[i].falling) {
      return true;
    }
  }
  return false;
}

bool SimBoard::matchTiles(int r1, int c1, int r2, int c2) const {
  const Cell &a = _cells[r1][c1];
  const Cell &b = _cells[r2][c2];
  return (a.type == BLOCK && b.type == BLOCK
      && (a.state == NORMAL || a.state == MATCHED)
      && (b.state == NORMAL || b.state == MATCHED)
      && !a.falling && !b.falling && a.color == b.color);
}

bool SimBoard::checkChain(int row, int col) const {
  for (int i = row; i >= 0; --i) {
    if (_cells[i][col].chain) {
      return true;
    }
  }
  return false;
}

bool SimBoard::allAirAbove(int row, int col) const {
  for (int i = row; i <= _topRow; ++i) {
    const Cell &c = _cells[i][col];
    if (c.type == AIR || c.type == GARBAGE
        || (c.type == BLOCK && c.state == NORMAL && !c.falling)) {
      continue;
    }
    return false;
  }
  return true;
}

void SimBoard::clearCell(Cell &c) {
  c.type = AIR;
  c.color = BlockColor::COUNT;
  c.g = -1;
  c.state = NORMAL;
  c.swapTimer = 0;
  c.floatTimer = 0;
  c.explTimer = 0;
  c.explTicks = 0;
  c.falling = 0;
  c.gbFall = 0;
}

bool SimBoard::canSwap(int x, int y) const {
  if (y < 0 || y >= Board::TOP_ROW || x < 0 || x + 1 >= W) {
    return false;
  }
  for (int col = x; col <= x + 1; ++col) {
    const Cell &c = _cells[y][col];
    bool swappable = ((c.type == AIR && _cells[y + 1][col].type != BLOCK)
        || c.type == BLOCK) && c.state == NORMAL;
    if (!swappable) {
      return false;
    }
  }
  return _cells[y][x].type == BLOCK || _cells[y][x + 1].type == BLOCK;
}

void SimBoard::startSwap(int x, int y) {
  if (!canSwap(x, y)) {
    return;
  }
  _cells[y][x].type = BLOCK;
  _cells[y][x + 1].type = BLOCK;
  _cells[y][x].state = SWAPPING_RIGHT;
  _cells[y][x + 1].state = SWAPPING_LEFT;
  _cells[y][x].swapTimer = 0;
  _cells[y][x + 1].swapTimer = 0;
  _changed = true;
  if (y > _topRow) {
    _topRow = y;
  }
}

void SimBoard::forceRaise() {
  if (!_activeBlocks) {
    _stackRaiseForced = true;
  }
}

int SimBoard::garbageForChain(int chain) const {
  int rows = chain - 1;
  if (rows > 12) {
    rows = 12;
  }
  return rows > 0 ? rows : 0;
}

// ---------------------------------------------------------------------------
// one tick, mirroring Board::tick()
// ---------------------------------------------------------------------------

void SimBoard::tick() {
  if (_state != Board::RUNNING) {
    return;
  }

  _tickMatched = 0;
  _activeBlocks = activeBlocks();
  if (!_activeBlocks && _chainCounter > 1) {
    _lastChain = _chainCounter; // chain end: sends garbage in VsGame
    _garbageSent += garbageForChain(_lastChain);
    _chainCounter = 1;
  }
  _blockOnTopRow = blockOnRow(Board::TOP_ROW);

  raiseStack();
  handleGarbageQueue();
  handleBlockTimers();

  // Nothing moved and nothing is animating: no fall, match or garbage trigger
  // can start, so most of the tick can be skipped. Matches only ever appear
  // after something changed, which is what makes this safe.
  if (_activeBlocks || _changed) {
    handleFalling();
    handleGarbageFalling();
    matchBlocks();
    handleMatchedBlocks();
    handleTriggeredBlocks();
  }
  _changed = false;

  if (_chainCounter > _maxChain) {
    _maxChain = _chainCounter;
  }

  // Board::sendEvents(): a combo of 4+ blocks scores twice here.
  if (_tickMatched > 3) {
    _score += chainScore(_chainCounter) + comboScore(_tickMatched, _chainCounter);
  }
  _garbageSent += comboGarbage(_tickMatched);

  updateTopRow();
}

void SimBoard::updateTopRow() {
  int row = _topRow;
  if (row >= H) {
    row = H - 1;
  }
  for (; row >= 0; --row) {
    for (int col = 0; col < W; ++col) {
      if (_cells[row][col].type != AIR) {
        _topRow = row;
        return;
      }
    }
  }
  _topRow = 0;
}

void SimBoard::raiseStack() {
  if (!_stackRaiseForced && _stackRaiseTimer < _stackRaiseTicks) {
    ++_stackRaiseTimer;
    return;
  }
  if (_stackOffset < Board::STACK_RAISE_STEPS && !_activeBlocks && !_blockOnTopRow) {
    ++_stackOffset;
    _stackRaiseTimer = 0;
  }
  if (_blockOnTopRow && !_activeBlocks) {
    if (++_graceTimer >= (Board::STACK_RAISE_STEPS * _stackRaiseTicks) / 2) {
      _state = Board::GAME_OVER;
    }
  }
  if (!_blockOnTopRow) {
    _graceTimer = 0;
  }

  if (_stackOffset >= Board::STACK_RAISE_STEPS) {
    if (!_blockOnTopRow && !_activeBlocks) {
      for (int i = 0; i < _numGarbage; ++i) {
        ++_garbage[i].y;
      }
      for (int row = H - 2; row >= 0; --row) {
        for (int col = 0; col < W; ++col) {
          _cells[row + 1][col] = _cells[row][col];
        }
      }
      for (int i = 0; i < W; ++i) {
        _cells[0][i] = _bufferRow[i];
      }
      fillBufferRow();
      _stackOffset = 0;
      if (_stackRaiseForced) {
        ++_score;
      }
      _stackRaiseForced = false;
      _topRow = _topRow + 1 < H ? _topRow + 1 : H - 1;
      _changed = true;
    }
  }
}

int SimBoard::spawnGarbage(int x, int y, int w, int h, GarbageBlockType type) {
  if (y >= H || x >= W || _numGarbage >= MAX_GARBAGE) {
    return -1;
  }
  int r = y - h;
  for (int row = r; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      if (_cells[row][col].type != AIR) {
        return -1;
      }
    }
  }

  Garbage &g = _garbage[_numGarbage];
  g.x = x;
  g.y = y;
  g.w = w;
  g.h = h;
  g.type = (type == GarbageBlockType::GRAY) ? 1 : 0;
  g.state = GS_NORMAL;
  g.transTicks = 0;
  g.transTimer = 0;
  g.animStart = 0;
  g.falling = 0;
  fillGarbageColors(g);

  int idx = _numGarbage++;
  for (int row = y; row >= y - (h - 1); --row) {
    for (int col = x; col <= x + (w - 1); ++col) {
      clearCell(_cells[row][col]);
      _cells[row][col].type = GARBAGE;
      _cells[row][col].g = (int8_t) idx;
    }
  }
  if (y > _topRow) {
    _topRow = y;
  }
  _changed = true;
  return idx;
}

void SimBoard::removeGarbage(int idx) {
  for (int i = idx; i < _numGarbage - 1; ++i) {
    _garbage[i] = _garbage[i + 1];
  }
  --_numGarbage;
  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == GARBAGE) {
        if (c.g == idx) {
          c.g = -1;
        } else if (c.g > idx) {
          --c.g;
        }
      }
    }
  }
}

void SimBoard::handleGarbageQueue() {
  if (_spawns.empty()) {
    return;
  }

  // lowest insertion point: the row above the highest occupied one
  int y = Board::TOP_ROW;
  for (int row = H - 1; row >= y; --row) {
    bool occupied = false;
    for (int col = 0; col < W; ++col) {
      if (_cells[row][col].type != AIR) {
        y = row + 1;
        occupied = true;
        break;
      }
    }
    if (occupied) {
      break;
    }
  }

  for (int i = 0; i < (int) _spawns.size(); ++i) {
    if (y >= H) {
      break;
    }
    Spawn &s = _spawns[i];
    if (--s.spawnTimer <= 0 && !_activeBlocks) {
      int x = 0;
      int w, h;
      if (s.fullWidth) {
        w = W;
        h = s.size;
        y += h - 1;
      } else {
        if (s.size == 3) {
          x = _spawnCounters[0]++ % 2 * 3;
        } else if (s.size < 6) {
          x = _spawnCounters[s.size - 3]++ % (W - s.size + 1);
        }
        w = s.size;
        h = 1;
      }
      if (spawnGarbage(x, y, w, h, s.type) >= 0) {
        ++y;
        _spawns.erase(_spawns.begin() + i);
        ++i; // Board::handleGarbageQueue() skips one entry after an erase
        continue;
      }
    }
  }
}

void SimBoard::swapBlocks(int row, int col) {
  Cell &swapRight = _cells[row][col];
  Cell &swapLeft = _cells[row][col + 1];
  if (swapLeft.state == SWAPPING_LEFT) {
    bool srChain = swapRight.chain;
    bool slChain = swapLeft.chain;
    Cell tmp = swapLeft;
    swapLeft = swapRight;
    swapRight = tmp;
    swapRight.chain = srChain;
    swapLeft.chain = slChain;
  }
  swapRight.swapTimer = 0;
  swapRight.state = NORMAL;
  swapLeft.swapTimer = 0;
  swapLeft.state = NORMAL;
}

void SimBoard::handleBlockTimers() {
  for (int row = 0; row <= _topRow; ++row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == BLOCK && c.state == EXPLODING) {
        ++c.explTimer;
        if (c.explTimer == c.explTicks) {
          _score += 10; // Board::handleBlockTimers() scores the explosion
          clearCell(c);
          c.chain = 1;
          _changed = true;
        }
      }
      if (c.state == SWAPPING_LEFT || c.state == SWAPPING_RIGHT) {
        ++c.swapTimer;
        if (c.swapTimer >= Board::SWAP_DELAY) {
          if (c.state == SWAPPING_RIGHT) {
            swapBlocks(row, col);
          }
          _changed = true;
        }
      }
    }
  }

  for (int i = 0; i < _numGarbage; ++i) {
    Garbage &g = _garbage[i];
    if (g.state != GS_TRANSFORMING) {
      continue;
    }
    ++g.transTimer;
    if (g.transTimer == g.transTicks) {
      g.transTicks = 0;
      g.transTimer = 0;
      g.state = GS_NORMAL;
      for (int k = 0; k < g.w; ++k) {
        int row = g.y - (g.h - 1);
        int col = g.x + k;
        Cell &c = _cells[row][col];
        clearCell(c);
        c.type = BLOCK;
        c.color = g.color[k];
        c.gbFall = 1;
        _changed = true;
      }
      if (--g.h <= 0) {
        removeGarbage(i);
        --i;
        continue;
      }
      fillGarbageColors(g);
    }
  }
}

bool SimBoard::garbageCanFall(const Garbage &g) const {
  if (g.state != GS_NORMAL) {
    return false;
  }
  // The row just below the block's bottom edge. A block resting on the board
  // floor has nothing under it and cannot fall further. (The real
  // Board::garbageBlockCanFall() reads _tiles[-1][col] in that case.)
  int row = g.y - g.h;
  if (row < 0) {
    return false;
  }
  for (int col = g.x; col <= g.x + (g.w - 1); ++col) {
    if (_cells[row][col].type != AIR) {
      return false;
    }
  }
  return true;
}

void SimBoard::handleFalling() {
  for (int col = 0; col < W; ++col) {
    for (int row = 0; row <= _topRow; ++row) {
      Cell &c = _cells[row][col];
      if (c.type == BLOCK && c.state == FLOATING) { // floating -> falling
        if (--c.floatTimer <= 0) {
          c.state = NORMAL;
          c.falling = 1;
          _changed = true;
          for (int y = row; y < H; ++y) {
            if (_cells[y][col].type == BLOCK) {
              _cells[y][col].falling = 1;
            }
          }
        }
      }
      bool canFall = (c.type == BLOCK && row > 0 && c.state == NORMAL
          && _cells[row - 1][col].type == AIR);
      if (canFall) {
        if (!c.falling) { // normal -> floating
          c.state = FLOATING;
          c.floatTimer = Board::FLOAT_TICKS;
          _changed = true;
        } else { // actual falling
          _changed = true;
          uint8_t destChain = _cells[row - 1][col].chain;
          uint8_t chain = c.chain;
          _cells[row - 1][col] = c;
          _cells[row - 1][col].chain = destChain;
          clearCell(_cells[row][col]);
          _cells[row][col].chain = chain;
        }
      } else if (c.falling) {
        c.falling = 0;
        _changed = true;
      }
    }
  }
}

void SimBoard::handleGarbageFalling() {
  for (int i = 0; i < _numGarbage; ++i) {
    Garbage &g = _garbage[i];
    if (garbageCanFall(g)) {
      g.falling = 1;
      _changed = true;
      int row = g.y - g.h;
      for (int col = g.x; col <= g.x + (g.w - 1); ++col) {
        clearCell(_cells[g.y][col]);
        if (row + 1 < H && _cells[row + 1][col].type == BLOCK) {
          _cells[row + 1][col].falling = 1;
        }
      }
      for (int col = g.x; col <= g.x + (g.w - 1); ++col) {
        _cells[row][col].type = GARBAGE;
        _cells[row][col].g = (int8_t) i;
      }
      --g.y;
    } else {
      g.falling = 0;
    }
  }
}

void SimBoard::matchBlocks() {
  for (int row = 0; row <= _topRow; ++row) {
    int matched = 1;
    int start = 0;
    for (int col = 1; col < W; ++col) {
      if (matchTiles(row, col, row, col - 1)) {
        ++matched;
      } else {
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
    if (matched >= 3) {
      for (int i = start; i < start + matched; ++i) {
        _cells[row][i].state = MATCHED;
        ++_tickMatched;
      }
      _changed = true;
    }
  }

  for (int col = 0; col < W; ++col) {
    int matched = 1;
    int start = 0;
    for (int row = 1; row <= _topRow; ++row) {
      if (matchTiles(row, col, row - 1, col)) {
        ++matched;
      } else {
        if (matched >= 3) {
          for (int i = start; i < start + matched; ++i) {
            _cells[i][col].state = MATCHED;
            ++_tickMatched;
          }
        }
        start = row;
        matched = 1;
      }
    }
    if (matched >= 3) {
      for (int i = start; i < start + matched; ++i) {
        _cells[i][col].state = MATCHED;
        ++_tickMatched;
      }
      _changed = true;
    }
  }
}

void SimBoard::handleMatchedBlocks() {
  bool tickChain = false;

  for (int row = _topRow; row >= 0; --row) {
    for (int col = 0; col < W; ++col) {
      Cell &c = _cells[row][col];
      if (c.type == BLOCK && c.state == MATCHED) {
        if (checkChain(row, col) || c.gbFall) {
          tickChain = true;
        }
        c.state = EXPLODING;
        c.explTimer = 0;
        c.explTicks = Board::BASE_EXPLOSION_TICKS + _tickMatched * Board::ADD_EXPL_TICKS;
        triggerNeighbors(row, col);
      }
      if ((c.type == BLOCK && c.state == NORMAL && !c.falling)
          || (c.type == AIR && allAirAbove(row, col))) {
        c.chain = 0;
        c.gbFall = 0;
      }
    }
  }

  if (tickChain) {
    ++_chainCounter;
    _score += chainScore(_chainCounter);
  }
}

void SimBoard::triggerNeighbors(int row, int col) {
  const int list[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
  for (int n = 0; n < 4; ++n) {
    triggerTile(row + list[n][0], col + list[n][1], -1);
  }
}

void SimBoard::triggerGarbageNeighbors(const Garbage &g, int triggerGarbageType) {
  for (int col = g.x; col <= g.x + (g.w - 1); ++col) {
    triggerTile(g.y + 1, col, triggerGarbageType);
    triggerTile(g.y - g.h, col, triggerGarbageType);
  }
  for (int row = g.y; row >= g.y - (g.h - 1); --row) {
    triggerTile(row, g.x - 1, triggerGarbageType);
    triggerTile(row, g.x + g.w, triggerGarbageType);
  }
}

void SimBoard::triggerTile(int row, int col, int triggerGarbageType) {
  if (row < 0 || col < 0 || row >= H || col >= W) {
    return;
  }
  Cell &c = _cells[row][col];
  if (c.type != GARBAGE || c.g < 0) {
    return;
  }
  Garbage &g = _garbage[c.g];
  if (g.state != GS_NORMAL) {
    return;
  }
  // gray garbage is only triggered by gray garbage
  if (triggerGarbageType < 0 || g.type == triggerGarbageType) {
    g.state = GS_TRIGGERED;
    triggerGarbageNeighbors(g, triggerGarbageType);
  }
}

void SimBoard::handleTriggeredBlocks() {
  int animStart = Board::BASE_EXPLOSION_TICKS + (_tickMatched + 1) * Board::ADD_EXPL_TICKS;
  bool marked[MAX_GARBAGE] = { false };

  for (int row = 0; row <= _topRow; ++row) {
    for (int col = W - 1; col >= 0; --col) {
      const Cell &c = _cells[row][col];
      if (c.type == GARBAGE && c.g >= 0 && _garbage[c.g].state == GS_TRIGGERED
          && !marked[c.g]) {
        marked[c.g] = true;
        _garbage[c.g].animStart = animStart;
        animStart += _garbage[c.g].w * _garbage[c.g].h * Board::ADD_EXPL_TICKS;
      }
    }
  }

  for (int i = 0; i < _numGarbage; ++i) {
    if (_garbage[i].state == GS_TRIGGERED && marked[i]) {
      _garbage[i].state = GS_TRANSFORMING;
      _garbage[i].transTimer = 0;
      _garbage[i].transTicks = _garbage[i].animStart;
    }
  }
}


