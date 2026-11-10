/*
 * ClaudeStrategy.cpp
 */

#include "ClaudeStrategy.h"

#include <SDL2/SDL_timer.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Game/Board.h"

namespace {

const double DEATH = -1e6;

// ---------------------------------------------------------------------------
// Timing-free chain resolver, used to judge a settled board by the best
// chain one more move could start ("chain potential"). It applies a move,
// lets everything fall, clears runs of three or more and repeats; every
// round after the first is a chain link. Garbage is a solid cell that does
// not fall. Close enough to the real thing for a settled board, and orders
// of magnitude cheaper than ticking a model through every candidate.
// ---------------------------------------------------------------------------

const int GR = 14;          // rows looked at; 0-11 are on screen
const int8_t EMPTY = -1;
const int8_t SOLID = 9;     // garbage

struct Grid {
  int8_t c[GR][6];
};

struct Resolve {
  int chain;     // rounds that cleared something (1 = a plain match)
  int cleared;   // blocks cleared
  int combo;     // blocks in the first round
  int garbage;   // cleared blocks touching garbage
};

inline bool isBlock(int8_t v) {
  return v >= 0 && v < SOLID;
}

void gravityCol(Grid &g, int col) {
  int write = 0;
  for (int row = 0; row < GR; ++row) {
    int8_t v = g.c[row][col];
    if (v == SOLID) {
      write = row + 1;
    } else if (v != EMPTY) {
      if (row != write) {
        g.c[write][col] = v;
        g.c[row][col] = EMPTY;
      }
      ++write;
    }
  }
}

// Marks runs of 3+ (bit col of mark[row]); returns the number of cells.
int findMatches(const Grid &g, uint8_t mark[GR], int maxRow) {
  int n = 0;
  memset(mark, 0, GR);
  for (int row = 0; row <= maxRow; ++row) {
    int run = 1;
    for (int col = 1; col <= 6; ++col) {
      if (col < 6 && isBlock(g.c[row][col]) && g.c[row][col] == g.c[row][col - 1]) {
        ++run;
        continue;
      }
      if (run >= 3) {
        for (int k = col - run; k < col; ++k) {
          mark[row] |= (uint8_t) (1 << k);
        }
      }
      run = 1;
    }
  }
  for (int col = 0; col < 6; ++col) {
    int run = 1;
    for (int row = 1; row <= maxRow + 1; ++row) {
      if (row <= maxRow && isBlock(g.c[row][col]) && g.c[row][col] == g.c[row - 1][col]) {
        ++run;
        continue;
      }
      if (run >= 3) {
        for (int k = row - run; k < row; ++k) {
          mark[k] |= (uint8_t) (1 << col);
        }
      }
      run = 1;
    }
  }
  for (int row = 0; row <= maxRow; ++row) {
    n += __builtin_popcount(mark[row]);
  }
  return n;
}

int topOf(const Grid &g) {
  for (int row = GR - 1; row >= 0; --row) {
    for (int col = 0; col < 6; ++col) {
      if (g.c[row][col] != EMPTY) {
        return row;
      }
    }
  }
  return -1;
}

Resolve resolve(Grid &g) {
  Resolve r = {0, 0, 0, 0};
  uint8_t mark[GR];
  int maxRow = topOf(g);
  while (maxRow >= 0) {
    int n = findMatches(g, mark, maxRow);
    if (n == 0) {
      break;
    }
    ++r.chain;
    r.cleared += n;
    if (r.chain == 1) {
      r.combo = n;
    }
    for (int row = 0; row <= maxRow; ++row) {
      if (!mark[row]) {
        continue;
      }
      for (int col = 0; col < 6; ++col) {
        if (!(mark[row] & (1 << col))) {
          continue;
        }
        if ((row + 1 < GR && g.c[row + 1][col] == SOLID) || (row > 0 && g.c[row - 1][col] == SOLID)
            || (col > 0 && g.c[row][col - 1] == SOLID) || (col < 5 && g.c[row][col + 1] == SOLID)) {
          ++r.garbage;
        }
        g.c[row][col] = EMPTY;
      }
    }
    for (int col = 0; col < 6; ++col) {
      gravityCol(g, col);
    }
  }
  return r;
}

// Board::swappable() on the static grid: an empty cell can't be swapped
// while a block sits on it, garbage never.
inline bool gridSwappable(const Grid &g, int row, int col) {
  int8_t v = g.c[row][col];
  if (v == SOLID) {
    return false;
  }
  if (v == EMPTY) {
    return !(row + 1 < GR && isBlock(g.c[row + 1][col]));
  }
  return true;
}

void gridFromSim(const ClaudeSim &m, Grid &g) {
  for (int row = 0; row < GR; ++row) {
    for (int col = 0; col < 6; ++col) {
      const ClaudeSim::Cell &c = m.cell(row, col);
      if (c.type == ClaudeSim::BLOCK) {
        g.c[row][col] = (c.state == EXPLODING || c.state == MATCHED) ? EMPTY : (int8_t) c.color;
      } else if (c.type == ClaudeSim::GARBAGE) {
        g.c[row][col] = SOLID;
      } else {
        g.c[row][col] = EMPTY;
      }
    }
  }
  for (int col = 0; col < 6; ++col) {
    gravityCol(g, col);
  }
}

// Garbage cells VsGame sends for a combo of this many blocks.
int comboCells(int combo) {
  if (combo >= 4 && combo <= 7) {
    return combo - 1;
  }
  if (combo == 8) {
    return 7;
  }
  if (combo >= 9 && combo <= 10) {
    return 2 * (combo - 5);
  }
  if (combo == 11) {
    return 11;
  }
  return 0;
}

bool debugEnabled() {
  static int on = -1;
  if (on < 0) {
    on = getenv("PANELPOP_CLAUDE_DEBUG") != nullptr ? 1 : 0;
  }
  return on == 1;
}

bool verifyEnabled() {
  static int on = -1;
  if (on < 0) {
    on = (getenv("PANELPOP_CLAUDE_VERIFY") != nullptr || debugEnabled()) ? 1 : 0;
  }
  return on == 1;
}

const char *actionName(AIStrategyAction a) {
  switch (a) {
    case AI_UP: return "U";
    case AI_DOWN: return "D";
    case AI_LEFT: return "L";
    case AI_RIGHT: return "R";
    case AI_SWAP: return "X";
    case AI_RAISE: return "^";
    case AI_WAIT: return ".";
  }
  return "?";
}

} // namespace

ClaudeStrategy::ClaudeStrategy() :
    _simulated(0),
    _lastSwapX(-1),
    _lastSwapY(-1),
    _lastSwapTick(0),
    _havePrediction(false),
    _verifyChecks(0),
    _verifyMisses(0) {
  _p.pace = 4;
  _p.fastPace = 3;
  _p.maxDrag = 3;
  _p.depth = 2;
  _p.width = 4;
  _p.budget = 60000;
  _p.waitCalm = 6;
  _p.waitActive = 3;
  _p.row = 100.0;
  _p.chainGrowth = 0.25;
  _p.potential = 0.6;
  _p.comboCell = 10.0;
  _p.clearCell = 8.0;
  _p.effort = 1.5;
  _p.flat = 4.0;
  _p.tower = 15.0;
  _p.low = 20.0;
  _p.comfort = 6.0;
  _p.danger = 25.0;
  _p.garbage = 2.0;
  _p.options = 3.0;
  _p.follow = 0.8;
  _p.undo = 40.0;
  _p.scramble = 80.0;
  const char *env = getenv("PANELPOP_CLAUDE_OPTIONS");
  if (env != nullptr) {
    setOptions(env);
  }
}

const std::string &ClaudeStrategy::name() const {
  static const std::string kName = "Claude";
  return kName;
}

const std::string &ClaudeStrategy::key() const {
  static const std::string kKey = "claude";
  return kKey;
}

void ClaudeStrategy::setOptions(const std::string &options) {
  struct IntKnob {
    const char *name;
    int Params::*field;
  };
  struct RealKnob {
    const char *name;
    double Params::*field;
  };
  static const IntKnob ints[] = {
      {"pace", &Params::pace}, {"fastPace", &Params::fastPace},
      {"drag", &Params::maxDrag}, {"depth", &Params::depth},
      {"width", &Params::width}, {"budget", &Params::budget},
      {"waitCalm", &Params::waitCalm}, {"waitActive", &Params::waitActive}};
  static const RealKnob reals[] = {
      {"row", &Params::row}, {"chainGrowth", &Params::chainGrowth},
      {"potential", &Params::potential}, {"comboCell", &Params::comboCell},
      {"clearCell", &Params::clearCell}, {"effort", &Params::effort},
      {"flat", &Params::flat}, {"tower", &Params::tower}, {"low", &Params::low},
      {"comfort", &Params::comfort}, {"danger", &Params::danger},
      {"garbage", &Params::garbage}, {"options", &Params::options},
      {"follow", &Params::follow}, {"undo", &Params::undo},
      {"scramble", &Params::scramble}};

  size_t pos = 0;
  while (pos < options.size()) {
    size_t end = options.find(',', pos);
    if (end == std::string::npos) {
      end = options.size();
    }
    const std::string item = options.substr(pos, end - pos);
    pos = end + 1;
    const size_t eq = item.find('=');
    if (eq == std::string::npos) {
      continue;
    }
    const std::string k = item.substr(0, eq);
    const double v = atof(item.c_str() + eq + 1);
    for (const IntKnob &knob : ints) {
      if (k == knob.name) {
        _p.*knob.field = (int) v;
      }
    }
    for (const RealKnob &knob : reals) {
      if (k == knob.name) {
        _p.*knob.field = v;
      }
    }
  }
  _p.pace = std::max(1, _p.pace);
  _p.fastPace = std::max(1, _p.fastPace);
  _p.maxDrag = std::max(1, std::min(5, _p.maxDrag));
}

void ClaudeStrategy::reset() {
  _havePrediction = false;
  _lastSwapX = _lastSwapY = -1;
  _lastSwapTick = 0;
}

// ---------------------------------------------------------------------------
// move generation
// ---------------------------------------------------------------------------

void ClaudeStrategy::appendAction(AIStrategyAction a, int pace,
                                  std::vector<AIStrategyAction> &out) {
  if (a == AI_SWAP) {
    // A block that is still swapping can't be swapped again: a swap applied
    // at tick k completes on tick k + 2, so the next one goes in at k + 3 at
    // the earliest (only matters below pace 2).
    const auto last = std::find(out.rbegin(), out.rend(), AI_SWAP);
    if (last != out.rend()) {
      for (int since = (int) (last - out.rbegin()); since + 1 < Board::SWAP_DELAY; ++since) {
        out.push_back(AI_WAIT);
      }
    }
  }
  out.push_back(a);
  for (int i = 1; i < pace; ++i) {
    out.push_back(AI_WAIT);
  }
}

void ClaudeStrategy::appendPath(int fromX, int fromY, int toX, int toY, int pace,
                             std::vector<AIStrategyAction> &out) const {
  // Vertical first: the stack shifting up under a travelling cursor moves
  // the cursor along with it, so the row stays right either way.
  while (fromY < toY) {
    appendAction(AI_UP, pace, out);
    ++fromY;
  }
  while (fromY > toY) {
    appendAction(AI_DOWN, pace, out);
    --fromY;
  }
  while (fromX < toX) {
    appendAction(AI_RIGHT, pace, out);
    ++fromX;
  }
  while (fromX > toX) {
    appendAction(AI_LEFT, pace, out);
    --fromX;
  }
}

void ClaudeStrategy::buildMoves(const ClaudeSim &from, std::vector<Move> &moves, int maxDrag,
                             int pace, bool withRaise) const {
  moves.clear();
  const int cx = from.cursorX();
  const int cy = from.cursorY();
  const int topRow = std::min((int) Board::TOP_ROW, from.top());

  for (int y = 0; y <= topRow; ++y) {
    for (int x = 0; x + 1 < ClaudeSim::W; ++x) {
      if (!from.canSwap(x, y)) {
        continue;
      }
      const ClaudeSim::Cell &a = from.cell(y, x);
      const ClaudeSim::Cell &b = from.cell(y, x + 1);
      if (a.type == b.type && a.color == b.color) {
        continue; // swapping two of the same does nothing
      }
      Move m;
      m.kind = MOVE_SWAP;
      m.x = x;
      m.y = y;
      m.dir = 0;
      m.len = 1;
      m.color = -1;
      appendPath(cx, cy, x, y, pace, m.actions);
      appendAction(AI_SWAP, pace, m.actions);
      moves.push_back(m);
    }
  }

  // Drags: one block slid len columns along its row, swap after swap.
  for (int y = 0; y <= topRow && maxDrag >= 2; ++y) {
    for (int x = 0; x < ClaudeSim::W; ++x) {
      const ClaudeSim::Cell &c = from.cell(y, x);
      if (c.type != ClaudeSim::BLOCK || c.state != NORMAL || c.falling) {
        continue;
      }
      for (int dir = -1; dir <= 1; dir += 2) {
        for (int len = 2; len <= maxDrag; ++len) {
          const int last = x + dir * len;
          if (last < 0 || last >= ClaudeSim::W) {
            break;
          }
          // The block must stay supported until the last step, or it drops
          // out of the drag; every cell on the way must be swappable.
          bool ok = true;
          for (int k = 1; k <= len && ok; ++k) {
            const int col = x + dir * k;
            const ClaudeSim::Cell &t = from.cell(y, col);
            if (t.type == ClaudeSim::GARBAGE || t.state != NORMAL
                || (t.type == ClaudeSim::AIR && from.cell(y + 1, col).type == ClaudeSim::BLOCK)) {
              ok = false;
            }
            if (k < len && y > 0 && from.cell(y - 1, col).type == ClaudeSim::AIR) {
              ok = false;
            }
          }
          if (!ok) {
            break;
          }
          Move m;
          m.kind = MOVE_DRAG;
          m.x = x;
          m.y = y;
          m.dir = dir;
          m.len = len;
          m.color = c.color;
          const int firstSwapX = dir > 0 ? x : x - 1;
          appendPath(cx, cy, firstSwapX, y, pace, m.actions);
          appendAction(AI_SWAP, pace, m.actions);
          for (int k = 1; k < len; ++k) {
            appendAction(dir > 0 ? AI_RIGHT : AI_LEFT, pace, m.actions);
            appendAction(AI_SWAP, pace, m.actions);
          }
          moves.push_back(m);
        }
      }
    }
  }

  if (withRaise) {
    Move m;
    m.kind = MOVE_RAISE;
    m.x = m.y = m.dir = m.len = 0;
    m.color = -1;
    appendAction(AI_RAISE, pace, m.actions);
    moves.push_back(m);
  }
}

// ---------------------------------------------------------------------------
// simulation
// ---------------------------------------------------------------------------

int ClaudeStrategy::inputs(const Move &move) {
  return (int) std::count_if(move.actions.begin(), move.actions.end(),
                             [](AIStrategyAction a) { return a != AI_WAIT; });
}

bool ClaudeStrategy::simulate(const ClaudeSim &from, const Move &move, ClaudeSim &m) {
  m = from;
  // The tick on which plan() runs: AIBoardController applies no action.
  m.tick();
  int swaps = 0;
  int row = move.y;
  int shifts = m.shifts();
  for (AIStrategyAction a : move.actions) {
    if (!m.running()) {
      break;
    }
    if (m.shifts() != shifts) {
      row += m.shifts() - shifts;   // the stack came up a row under us
      shifts = m.shifts();
    }
    if (a == AI_SWAP) {
      if (move.kind == MOVE_DRAG && swaps > 0) {
        // The dragged block has to be where the last swap put it, at rest.
        const int col = move.x + move.dir * swaps;
        const ClaudeSim::Cell &c = m.cell(row, col);
        if (c.type != ClaudeSim::BLOCK || c.color != move.color || c.state != NORMAL || c.falling) {
          return false;
        }
      }
      if (!m.swapAtCursor()) {
        return false;
      }
      ++swaps;
      m.tick();
    } else {
      m.step(a);
    }
  }
  _simulated += m.simTicks();
  rollout(m);
  return true;
}

void ClaudeStrategy::rollout(ClaudeSim &m) {
  // Until the board is at rest and any chain has been counted (the chain
  // ends on the first tick that starts with nothing active).
  const int cap = 1500;
  int t = 0;
  while (m.running() && t < cap) {
    if (!m.anyActive() && m.chainCounter() <= 1 && !m.raiseForced()) {
      break;
    }
    m.tick();
    ++t;
  }
  _simulated += t;
}

// ---------------------------------------------------------------------------
// evaluation
// ---------------------------------------------------------------------------

double ClaudeStrategy::chainReward(int rows) const {
  if (rows <= 0) {
    return 0;
  }
  return _p.row * rows * (1.0 + _p.chainGrowth * (rows - 1));
}

double ClaudeStrategy::potential(const ClaudeSim &m, int &triggers) const {
  Grid g;
  gridFromSim(m, g);
  triggers = 0;
  double best = 0;
  const int top = std::min(topOf(g), (int) Board::TOP_ROW);

  auto score = [this](const Resolve &r) {
    double v = chainReward(r.chain - 1);
    v += _p.comboCell * comboCells(r.combo);
    if (r.garbage > 0) {
      v += _p.clearCell * 6;
    }
    return v;
  };

  uint8_t mark[GR];
  for (int y = 0; y <= top; ++y) {
    for (int x = 0; x < 5; ++x) {
      const int8_t a = g.c[y][x];
      const int8_t b = g.c[y][x + 1];
      if (a == b || !gridSwappable(g, y, x) || !gridSwappable(g, y, x + 1)) {
        continue;
      }
      Grid t = g;
      t.c[y][x] = b;
      t.c[y][x + 1] = a;
      gravityCol(t, x);
      gravityCol(t, x + 1);
      if (findMatches(t, mark, std::min(GR - 1, top + 1)) == 0) {
        continue;
      }
      Resolve r = resolve(t);
      if (r.chain >= 2) {
        ++triggers;
      }
      best = std::max(best, score(r));
      if (r.chain >= 1) {
        ++triggers; // any available match keeps the stack flexible
      }
    }
  }

  // Triggers that need a block slid two or three columns.
  for (int y = 0; y <= top; ++y) {
    for (int x = 0; x < 6; ++x) {
      if (!isBlock(g.c[y][x])) {
        continue;
      }
      for (int dir = -1; dir <= 1; dir += 2) {
        Grid t = g;
        int pos = x;
        for (int step = 1; step <= 3; ++step) {
          const int nx = pos + dir;
          if (nx < 0 || nx > 5 || !gridSwappable(t, y, nx) || t.c[y][pos] != g.c[y][x]) {
            break;
          }
          std::swap(t.c[y][pos], t.c[y][nx]);
          Grid s = t;
          gravityCol(s, pos);
          gravityCol(s, nx);
          const bool fell = y > 0 && s.c[y][nx] != g.c[y][x];
          if (findMatches(s, mark, std::min(GR - 1, top + 1)) > 0) {
            if (step >= 2) {
              Resolve r = resolve(s);
              if (r.chain >= 2) {
                ++triggers;
              }
              best = std::max(best, score(r) - 2.0 * _p.effort * step);
            }
            break;
          }
          if (fell) {
            break;
          }
          t = s;
          pos = nx;
        }
      }
    }
  }
  return best;
}

double ClaudeStrategy::evaluate(const ClaudeSim &m) const {
  if (m.dead()) {
    return DEATH + 50.0 * m.simTicks();
  }
  const ClaudeSim::Stats &s = m.stats();
  double v = 0;

  // What actually happened.
  for (int i = 0; i < s.numChains; ++i) {
    v += chainReward(s.chainLen[i] - 1);
  }
  if (m.chainCounter() > 1) {
    v += 0.9 * chainReward(m.chainCounter() - 1); // still running at the cap
  }
  v += _p.comboCell * s.comboCells;
  v += _p.clearCell * s.garbageCleared;

  // The settled board.
  int h[ClaudeSim::W];
  int garbageCells = 0;
  int blocks = 0;
  for (int col = 0; col < ClaudeSim::W; ++col) {
    h[col] = 0;
    for (int row = 0; row < ClaudeSim::H; ++row) {
      const ClaudeSim::Cell &c = m.cell(row, col);
      if (c.type != ClaudeSim::AIR) {
        h[col] = row + 1;
        if (c.type == ClaudeSim::GARBAGE) {
          ++garbageCells;
        } else if (row <= Board::TOP_ROW && c.state != EXPLODING) {
          ++blocks;
        }
      }
    }
  }

  // Garbage on its way down lands on top of everything.
  double incoming = 0;
  for (int i = 0; i < m.numSpawns(); ++i) {
    const ClaudeSim::Spawn &sp = m.spawn(i);
    incoming += sp.fullWidth ? sp.size : sp.size / 6.0;
  }
  const double raise = m.stackOffset() / (double) Board::STACK_RAISE_STEPS;

  for (int col = 0; col < ClaudeSim::W; ++col) {
    const double eff = h[col] + raise + incoming;
    if (eff > 7.0) {
      v -= _p.danger * (eff - 7.0) * (eff - 7.0);
    }
    if (col + 1 < ClaudeSim::W) {
      v -= _p.flat * std::abs(h[col] - h[col + 1]);
    }
    const int left = col > 0 ? h[col - 1] : h[col + 1];
    const int right = col + 1 < ClaudeSim::W ? h[col + 1] : h[col - 1];
    const int over = h[col] - std::max(left, right);
    if (over >= 3) {
      v -= _p.tower * (over - 2);
    }
  }
  // Material: blocks to build chains with and to dig garbage out with.
  // Garbage does not count, which is the point - a stack of garbage over
  // a handful of blocks is the most dangerous board there is.
  const double comfortBlocks = _p.comfort * ClaudeSim::W;
  if (blocks < comfortBlocks) {
    v -= _p.low * (comfortBlocks - blocks) / ClaudeSim::W;
  }
  if (m.graceTimer() > 0) {
    v -= 600.0 + 40.0 * m.graceTimer();
  }
  v -= _p.garbage * garbageCells;

  int triggers = 0;
  v += _p.potential * potential(m, triggers);
  v += _p.options * std::min(triggers, 6);
  return v;
}

// ---------------------------------------------------------------------------
// plan
// ---------------------------------------------------------------------------

bool ClaudeStrategy::predictionUsable(const ClaudeSim &now) const {
  if (!_havePrediction || _prediction.inventedColors()) {
    return false;
  }
  // Garbage the opponent sent in the meantime is news, not a model error.
  return now.numSpawns() == _prediction.numSpawns();
}

std::vector<AIStrategyAction> ClaudeStrategy::plan(const Board &board) {
  std::vector<AIStrategyAction> actions;
  if (board.getState() != Board::RUNNING) {
    return actions;
  }
  const Uint32 started = SDL_GetTicks();

  ClaudeSim base;
  base.import(board);

  if (verifyEnabled() && predictionUsable(base)) {
    ++_verifyChecks;
    const char *what = nullptr;
    for (int r = 0; r < ClaudeSim::H && what == nullptr; ++r) {
      for (int c = 0; c < ClaudeSim::W && what == nullptr; ++c) {
        const ClaudeSim::Cell &a = base.cell(r, c);
        const ClaudeSim::Cell &b = _prediction.cell(r, c);
        if (a.type != b.type || a.color != b.color || a.state != b.state
            || a.falling != b.falling || a.chain != b.chain) {
          what = "cells";
        }
      }
    }
    if (what == nullptr && (base.cursorX() != _prediction.cursorX()
        || base.cursorY() != _prediction.cursorY())) {
      what = "cursor";
    }
    if (what == nullptr && base.stackOffset() != _prediction.stackOffset()) {
      what = "stack";
    }
    if (what == nullptr && base.score() != _prediction.score()) {
      what = "score";
    }
    if (what != nullptr) {
      ++_verifyMisses;
      printf("claude: tick %u prediction miss (%s), %ld of %ld\n",
             board.getTicksRun(), what, _verifyMisses, _verifyChecks);
      fflush(stdout);
    }
  }
  _havePrediction = false;

  _simulated = 0;
  const bool active = base.anyActive() || base.chainCounter() > 1;
  int maxH = 0;
  int blocks = 0;
  int garbageCells = 0;
  for (int col = 0; col < ClaudeSim::W; ++col) {
    for (int row = Board::TOP_ROW; row >= 0; --row) {
      const ClaudeSim::Cell &c = base.cell(row, col);
      if (c.type != ClaudeSim::AIR) {
        maxH = std::max(maxH, row + 1);
      }
      blocks += c.type == ClaudeSim::BLOCK;
      garbageCells += c.type == ClaudeSim::GARBAGE;
    }
  }
  // About to top out, or buried under garbage with little to dig with: a
  // player fights on here instead of watching it happen.
  const bool desperate = base.graceTimer() > 0 || maxH >= Board::TOP_ROW
      || (garbageCells >= 12 && blocks < 18);
  const bool hurry = active || maxH >= Board::PANIC_HEIGHT || base.numSpawns() > 0;
  const int pace = hurry ? _p.fastPace : _p.pace;

  std::vector<Move> moves;
  buildMoves(base, moves, _p.maxDrag, pace, !base.anyActive() && !base.raiseForced());
  {
    Move wait;
    wait.kind = MOVE_WAIT;
    wait.x = wait.y = wait.dir = wait.len = 0;
    wait.color = -1;
    wait.actions.assign(active ? _p.waitActive : _p.waitCalm, AI_WAIT);
    moves.push_back(wait);
  }

  std::vector<Line> lines;
  lines.reserve(moves.size());
  for (const Move &mv : moves) {
    Line line;
    line.move = mv;
    line.valid = simulate(base, mv, line.end);
    if (!line.valid) {
      continue;
    }
    line.value = evaluate(line.end) - _p.effort * inputs(mv);
    line.total = line.value;
    lines.push_back(line);
  }
  if (lines.empty()) {
    actions.assign(_p.waitActive, AI_WAIT);
    return actions;
  }

  // Flipping the pair just swapped straight back looks like flailing; only
  // do it when it makes something happen that waiting would not.
  int waitMatches = 0;
  for (const Line &line : lines) {
    if (line.move.kind == MOVE_WAIT) {
      waitMatches = line.end.stats().matches;
    }
  }
  for (Line &line : lines) {
    if (desperate && line.move.kind == MOVE_WAIT) {
      line.value -= _p.scramble;
      line.total = line.value;
    }
    if (line.move.kind == MOVE_SWAP && line.move.x == _lastSwapX && line.move.y == _lastSwapY
        && board.getTicksRun() - _lastSwapTick < 60
        && line.end.stats().matches <= waitMatches) {
      line.value -= _p.undo;
      line.total = line.value;
    }
  }
  std::sort(lines.begin(), lines.end(), [](const Line &a, const Line &b) {
    return a.value > b.value;
  });

  // Second move on top of the best few: setups that pay off a move later.
  // Only on a calm board: during a chain the single-move search already
  // times every swap exactly, which is what extending the chain needs.
  if (_p.depth >= 2 && !active) {
    const int width = std::min((int) lines.size(), _p.width);
    std::vector<Move> next;
    for (int i = 0; i < width && _simulated < _p.budget; ++i) {
      Line &line = lines[i];
      if (!line.end.running()) {
        continue;
      }
      buildMoves(line.end, next, std::min(_p.maxDrag, 2), pace,
                 !line.end.anyActive());
      double best = line.value;
      ClaudeSim after;
      for (const Move &mv : next) {
        if (_simulated >= _p.budget) {
          break;
        }
        if (!simulate(line.end, mv, after)) {
          continue;
        }
        best = std::max(best, evaluate(after) - _p.effort * inputs(mv));
      }
      line.total = line.value + _p.follow * (best - line.value);
    }
    std::stable_sort(lines.begin(), lines.end(), [](const Line &a, const Line &b) {
      return a.total > b.total;
    });
  }

  const Line &best = lines.front();
  actions = best.move.actions;
  if (best.move.kind == MOVE_SWAP) {
    _lastSwapX = best.move.x;
    _lastSwapY = best.move.y;
    _lastSwapTick = board.getTicksRun();
  }

  if (verifyEnabled()) {
    // Where the board will be when we are asked again: the planning tick
    // plus one tick per action, no rollout.
    _prediction = base;
    _prediction.tick();
    for (AIStrategyAction a : actions) {
      _prediction.step(a);
    }
    _havePrediction = true;
  }
  if (debugEnabled()) {
    describe(best, base, SDL_GetTicks() - started);
    // A move made while a chain runs that makes it longer than just
    // watching it would: a skill chain link.
    int waitChain = 0;
    for (const Line &line : lines) {
      if (line.move.kind == MOVE_WAIT) {
        waitChain = line.end.stats().maxChain;
      }
    }
    if (base.chainCounter() > 1 && best.move.kind != MOVE_WAIT
        && best.end.stats().maxChain > waitChain) {
      printf("claude: skill+%d at chain %d\n", best.end.stats().maxChain - waitChain,
             base.chainCounter());
    }
  }
  return actions;
}

void ClaudeStrategy::describe(const Line &line, const ClaudeSim &base, unsigned ms) const {
  static const char *kinds[] = {"swap", "drag", "raise", "wait"};
  const ClaudeSim::Stats &s = line.end.stats();
  std::string acts;
  for (AIStrategyAction a : line.move.actions) {
    if (a != AI_WAIT) {
      acts += actionName(a);
    }
  }
  int triggers = 0;
  const double pot = potential(line.end, triggers);
  printf("claude: t=%u chain=%d %s(%d,%d%s%d) v=%.0f tot=%.0f sent=%d+%dc clr=%d maxc=%d pot=%.0f trig=%d sims=%d ms=%u [%s]\n",
         base.ticksRun(), base.chainCounter(), kinds[line.move.kind], line.move.x, line.move.y,
         line.move.dir > 0 ? ">" : (line.move.dir < 0 ? "<" : ""), line.move.len,
         line.value, line.total, s.chainRows, s.comboCells, s.garbageCleared, s.maxChain,
         pot, triggers, _simulated, ms, acts.c_str());
  fflush(stdout);
}
