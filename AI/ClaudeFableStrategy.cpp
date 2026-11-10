/*
 * ClaudeFableStrategy.cpp
 */

#include "ClaudeFableStrategy.h"

#include <SDL2/SDL_timer.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>

#include "../Game/Board.h"

namespace {

const int W = ClaudeSim::W;
const int ROWS = Board::TOP_ROW + 1;   // rows the cursor can reach
const int BRIM_ROW = Board::TOP_ROW - 1;
const int NEVER = 1 << 20;
const double LOST = -1e6;
const double DEAD = -1e9;

// ---------------------------------------------------------------------------
// Timing-free chain resolver for a board at rest: apply a move, let blocks
// fall, clear runs of three or more, repeat. Every round after the first is
// a chain link. Garbage is a solid cell. Used to judge what a settled board
// promises; everything that is actually played goes through ClaudeSim.
// ---------------------------------------------------------------------------

const int GR = 14;
const int8_t EMPTY = -1;
const int8_t SOLID = 9;

struct Grid {
  int8_t c[GR][W];
};

inline bool isBlock(int8_t v) {
  return v >= 0 && v < SOLID;
}

void gravityCol(Grid &g, int col) {
  int write = 0;
  for (int row = 0; row < GR; ++row) {
    const int8_t v = g.c[row][col];
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

int findMatches(const Grid &g, uint8_t mark[GR]) {
  int n = 0;
  memset(mark, 0, GR);
  for (int row = 0; row < GR; ++row) {
    int run = 1;
    for (int col = 1; col <= W; ++col) {
      if (col < W && isBlock(g.c[row][col]) && g.c[row][col] == g.c[row][col - 1]) {
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
  for (int col = 0; col < W; ++col) {
    int run = 1;
    for (int row = 1; row <= GR; ++row) {
      if (row < GR && isBlock(g.c[row][col]) && g.c[row][col] == g.c[row - 1][col]) {
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
  for (int row = 0; row < GR; ++row) {
    n += __builtin_popcount(mark[row]);
  }
  return n;
}

// True when the cell is part of a run of three or more.
bool inRun(const Grid &g, int row, int col) {
  const int8_t v = g.c[row][col];
  if (!isBlock(v)) {
    return false;
  }
  int n = 1;
  for (int k = col - 1; k >= 0 && g.c[row][k] == v; --k) {
    ++n;
  }
  for (int k = col + 1; k < W && g.c[row][k] == v; ++k) {
    ++n;
  }
  if (n >= 3) {
    return true;
  }
  n = 1;
  for (int k = row - 1; k >= 0 && g.c[k][col] == v; --k) {
    ++n;
  }
  for (int k = row + 1; k < GR && g.c[k][col] == v; ++k) {
    ++n;
  }
  return n >= 3;
}

struct Resolve {
  int chain;     // rounds that cleared something
  int cleared;
  bool garbage;  // a cleared block touched garbage
};

Resolve resolve(Grid &g) {
  Resolve r = {0, 0, false};
  uint8_t mark[GR];
  for (;;) {
    const int n = findMatches(g, mark);
    if (n == 0) {
      break;
    }
    ++r.chain;
    r.cleared += n;
    for (int row = 0; row < GR; ++row) {
      if (!mark[row]) {
        continue;
      }
      for (int col = 0; col < W; ++col) {
        if (!(mark[row] & (1 << col))) {
          continue;
        }
        if ((row + 1 < GR && g.c[row + 1][col] == SOLID) || (row > 0 && g.c[row - 1][col] == SOLID)
            || (col > 0 && g.c[row][col - 1] == SOLID) || (col + 1 < W && g.c[row][col + 1] == SOLID)) {
          r.garbage = true;
        }
        g.c[row][col] = EMPTY;
      }
    }
    for (int col = 0; col < W; ++col) {
      gravityCol(g, col);
    }
  }
  return r;
}

inline bool gridSwappable(const Grid &g, int row, int col) {
  const int8_t v = g.c[row][col];
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
    for (int col = 0; col < W; ++col) {
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
  for (int col = 0; col < W; ++col) {
    gravityCol(g, col);
  }
}

// Links the best single swap or short drag would set off on this grid
// (clearing next to garbage counts for half a link: the blocks it turns into
// fall as part of the chain), and how many matches are available at all.
double potential(const Grid &g, int &triggers) {
  triggers = 0;
  double best = 0;
  auto worth = [](const Resolve &r) {
    return (r.chain - 1) + (r.garbage ? 0.5 : 0.0);
  };
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x + 1 < W; ++x) {
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
      // Only a match this swap makes itself; the grid has none before it.
      bool hit = false;
      for (int row = 0; row < GR && !hit; ++row) {
        hit = inRun(t, row, x) || inRun(t, row, x + 1);
      }
      if (!hit) {
        continue;
      }
      ++triggers;
      best = std::max(best, worth(resolve(t)));
    }
  }
  // A block slid two or three columns along its row.
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < W; ++x) {
      if (!isBlock(g.c[y][x])) {
        continue;
      }
      for (int dir = -1; dir <= 1; dir += 2) {
        Grid t = g;
        int pos = x;
        for (int n = 1; n <= 3; ++n) {
          const int nx = pos + dir;
          if (nx < 0 || nx >= W || !gridSwappable(t, y, nx) || t.c[y][pos] != g.c[y][x]) {
            break;
          }
          std::swap(t.c[y][pos], t.c[y][nx]);
          Grid s = t;
          gravityCol(s, pos);
          gravityCol(s, nx);
          const bool fell = s.c[y][nx] != g.c[y][x];
          bool hit = false;
          for (int row = 0; row < GR && !hit; ++row) {
            hit = inRun(s, row, pos) || inRun(s, row, nx);
          }
          if (hit) {
            if (n >= 2) {
              best = std::max(best, worth(resolve(s)) - 0.05 * n);
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

// Rows of garbage waiting in the queue.
int pendingRows(const ClaudeSim &m) {
  int rows = 0;
  for (int i = 0; i < m.numSpawns(); ++i) {
    rows += m.spawn(i).fullWidth ? m.spawn(i).size : 1;
  }
  return rows;
}

bool debugEnabled() {
  static int on = -1;
  if (on < 0) {
    on = getenv("PANELPOP_FABLE_DEBUG") != nullptr ? 1 : 0;
  }
  return on == 1;
}

inline bool resting(const ClaudeSim &m, int row, int col) {
  const ClaudeSim::Cell &c = m.cell(row, col);
  return c.type == ClaudeSim::BLOCK && c.state == NORMAL && !c.falling;
}

inline bool supported(const ClaudeSim &m, int row, int col) {
  return row == 0 || m.cell(row - 1, col).type == ClaudeSim::GARBAGE || resting(m, row - 1, col);
}

} // namespace

// ---------------------------------------------------------------------------
// Cursor travel that keeps the board active.
//
// "free" is the number of coming ticks that are fine without a new swap
// (something is still exploding, falling or swapping, or nothing needs to be
// kept alive at all). A cursor step uses one up; a swap is good for the tick
// it is made on and the two after it. So a far target is reached by hopping:
// swap, two steps, swap, two steps... The hops are made on pairs where a swap
// has no consequences if there are any on the way: two blocks of the same
// colour (nothing changes), else two resting blocks that match nothing either
// way round, else anything that can be swapped at all.
// ---------------------------------------------------------------------------

struct ClaudeFableStrategy::Travel {
  static const int FMAX = 16;   // 16 = more than any path needs
  static const int STATES = ROWS * (W - 1) * (FMAX + 1);

  uint8_t hop[ROWS][W - 1];     // 0 no swap, 1 same colour, 2 harmless, 3 legal
  int dist[STATES];
  int from[STATES];
  uint8_t act[STATES];

  static int index(int x, int y, int f) {
    return (y * (W - 1) + x) * (FMAX + 1) + f;
  }

  void classify(const ClaudeSim &m) {
    Grid g;
    for (int row = 0; row < GR; ++row) {
      for (int col = 0; col < W; ++col) {
        g.c[row][col] = resting(m, row, col) ? (int8_t) m.cell(row, col).color : EMPTY;
      }
    }
    for (int y = 0; y < ROWS; ++y) {
      for (int x = 0; x + 1 < W; ++x) {
        uint8_t kind = 0;
        if (resting(m, y, x) && resting(m, y, x + 1) && supported(m, y, x) && supported(m, y, x + 1)) {
          if (g.c[y][x] == g.c[y][x + 1]) {
            kind = 1;
          } else {
            std::swap(g.c[y][x], g.c[y][x + 1]);
            kind = (inRun(g, y, x) || inRun(g, y, x + 1)) ? 3 : 2;
            std::swap(g.c[y][x], g.c[y][x + 1]);
          }
        } else if (m.canSwap(x, y)) {
          kind = 3;
        }
        hop[y][x] = kind;
      }
    }
  }

  void build(const ClaudeSim &m, int free) {
    classify(m);
    std::fill(dist, dist + STATES, NEVER);
    typedef std::pair<int, int> Item;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item> > queue;
    const int start = index(std::min(m.cursorX(), W - 2), std::min(m.cursorY(), ROWS - 1),
                            std::min(free, (int) FMAX));
    dist[start] = 0;
    from[start] = -1;
    queue.push(Item(0, start));
    static const int DX[4] = {0, 1, 0, -1};
    static const int DY[4] = {1, 0, -1, 0};
    static const AIStrategyAction MOVE[4] = {AI_UP, AI_RIGHT, AI_DOWN, AI_LEFT};
    static const int HOP_COST[4] = {0, 1, 6, 200};
    while (!queue.empty()) {
      const Item top = queue.top();
      queue.pop();
      const int s = top.second;
      if (top.first != dist[s]) {
        continue;
      }
      const int f = s % (FMAX + 1);
      const int cell = s / (FMAX + 1);
      const int x = cell % (W - 1);
      const int y = cell / (W - 1);
      auto relax = [&](int nx, int ny, int nf, int cost, AIStrategyAction a) {
        const int n = index(nx, ny, nf);
        if (dist[s] + cost < dist[n]) {
          dist[n] = dist[s] + cost;
          from[n] = s;
          act[n] = (uint8_t) a;
          queue.push(Item(dist[n], n));
        }
      };
      if (f >= 1) {
        const int nf = f == FMAX ? FMAX : f - 1;
        for (int d = 0; d < 4; ++d) {
          const int nx = x + DX[d];
          const int ny = y + DY[d];
          if (nx >= 0 && nx < W - 1 && ny >= 0 && ny < ROWS) {
            relax(nx, ny, nf, 16, MOVE[d]);
          }
        }
      }
      if (hop[y][x] != 0 && f < FMAX) {
        relax(x, y, std::max(f - 1, 2), 16 + HOP_COST[hop[y][x]], AI_SWAP);
      }
    }
  }

  // Cheapest way to (x, y); returns the cost and the free ticks left there.
  int best(int x, int y, int &f) const {
    int cost = NEVER;
    f = 0;
    for (int k = FMAX; k >= 0; --k) {
      if (dist[index(x, y, k)] < cost) {
        cost = dist[index(x, y, k)];
        f = k;
      }
    }
    return cost;
  }

  bool path(int x, int y, std::vector<AIStrategyAction> &out) const {
    int f;
    if (best(x, y, f) >= NEVER) {
      return false;
    }
    const size_t first = out.size();
    for (int s = index(x, y, f); from[s] >= 0; s = from[s]) {
      out.push_back((AIStrategyAction) act[s]);
    }
    std::reverse(out.begin() + first, out.end());
    return true;
  }
};

// ---------------------------------------------------------------------------

ClaudeFableStrategy::ClaudeFableStrategy() :
    _holdChain(true),
    _releasing(false),
    _stuck(false),
    _lastScore(0),
    _lastProgressTick(0),
    _reserve(0),
    _pendingRows(0),
    _lastChain(1),
    _lastLinkTick(0),
    _plans(0),
    _lostChains(0) {
  _p.hold = 1;
  _p.target = 13;
  _p.patience = 600;
  _p.bored = 150;
  _p.maxDrag = 3;
  _p.raiseTop = 10;
  _p.width = 4;
  _p.budget = 10000;
  _p.busyWait = 5;
  _p.reserve = 24;
  _p.low = 20.0;
  _p.link = 100.0;
  _p.pot = 45.0;
  _p.option = 2.0;
  _p.material = 2.0;
  _p.overflow = 30.0;
  _p.stock = 50;
  _p.breach = 60.0;
  _p.margin = 2.0;
  _p.effort = 0.5;
  _p.clearCell = 6.0;
  _p.comboCell = 2.0;
  _p.garbage = 1.0;
  _p.tick = 0.2;
  _p.follow = 0.7;
  const char *env = getenv("PANELPOP_FABLE_OPTIONS");
  if (env != nullptr) {
    setOptions(env);
  }
}

const std::string &ClaudeFableStrategy::name() const {
  static const std::string kName = "ClaudeFable";
  return kName;
}

const std::string &ClaudeFableStrategy::key() const {
  static const std::string kKey = "claudefable";
  return kKey;
}

void ClaudeFableStrategy::setOptions(const std::string &options) {
  struct IntKnob {
    const char *name;
    int Params::*field;
  };
  struct RealKnob {
    const char *name;
    double Params::*field;
  };
  static const IntKnob ints[] = {
      {"hold", &Params::hold}, {"target", &Params::target},
      {"patience", &Params::patience}, {"bored", &Params::bored}, {"drag", &Params::maxDrag},
      {"raiseTop", &Params::raiseTop}, {"width", &Params::width},
      {"budget", &Params::budget}, {"busyWait", &Params::busyWait},
      {"reserve", &Params::reserve}, {"stock", &Params::stock}};
  static const RealKnob reals[] = {
      {"link", &Params::link}, {"pot", &Params::pot}, {"option", &Params::option},
      {"material", &Params::material}, {"overflow", &Params::overflow},
      {"clearCell", &Params::clearCell}, {"comboCell", &Params::comboCell},
      {"garbage", &Params::garbage}, {"tick", &Params::tick},
      {"follow", &Params::follow}, {"low", &Params::low},
      {"breach", &Params::breach}, {"margin", &Params::margin},
      {"effort", &Params::effort}};

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
  _p.maxDrag = std::max(1, std::min(5, _p.maxDrag));
  _p.raiseTop = std::max(3, std::min((int) Board::TOP_ROW - 1, _p.raiseTop));
  _p.target = std::max(2, _p.target);
}

void ClaudeFableStrategy::reset() {
  _releasing = false;
  _stuck = false;
  _lastScore = 0;
  _lastProgressTick = 0;
  _lastChain = 1;
  _lastLinkTick = 0;
}

// ---------------------------------------------------------------------------
// simulation
// ---------------------------------------------------------------------------

// One controller tick plus one board tick. Returns true when the tick was
// one that must not happen: it ended a chain we are holding, or it ran the
// top-out grace timer (or killed the board).
bool ClaudeFableStrategy::step(ClaudeSim &m, AIStrategyAction a, bool &swapped) const {
  const int chain = m.chainCounter();
  const int grace = m.graceTimer();
  swapped = true;
  if (a == AI_SWAP) {
    swapped = m.swapAtCursor();
    m.tick();
  } else {
    m.step(a);
  }
  if (m.dead()) {
    return true;
  }
  return !m.activeFlag() && ((_holdChain && chain > 1) || m.graceTimer() > grace);
}

void ClaudeFableStrategy::rollout(ClaudeSim &m) const {
  for (int t = 0; t < 1500 && m.running() && m.anyActive(); ++t) {
    m.tick();
  }
}

// After a move the board has to stay fine through the next planning tick
// (on which no input is made), and the plan after that needs a way to carry
// on: either nothing is urgent, or the cursor sits on something to swap.
bool ClaudeFableStrategy::continuable(const ClaudeSim &m) const {
  ClaudeSim t = m;
  bool swapped;
  for (int i = 0; i < 8; ++i) {
    ClaudeSim before = t;
    if (step(t, AI_WAIT, swapped)) {
      return i >= 1 && before.canSwap(before.cursorX(), before.cursorY());
    }
  }
  return true;
}

// Ends a move that leaves nothing active by walking to a nearby pair that
// can be swapped back and forth without consequences and starting to do so.
// (Swapping the pair of the move itself again would undo the move.)
bool ClaudeFableStrategy::park(ClaudeSim &m, std::vector<AIStrategyAction> &actions,
                               int tx, int ty) const {
  static const AIStrategyAction OPTIONS[][2] = {
      {AI_UP, AI_WAIT}, {AI_DOWN, AI_WAIT}, {AI_UP, AI_UP}, {AI_DOWN, AI_DOWN},
      {AI_UP, AI_RIGHT}, {AI_UP, AI_LEFT}, {AI_DOWN, AI_RIGHT}, {AI_DOWN, AI_LEFT},
      {AI_RIGHT, AI_RIGHT}, {AI_LEFT, AI_LEFT}, {AI_RIGHT, AI_WAIT}, {AI_LEFT, AI_WAIT}};
  bool swapped;
  for (const AIStrategyAction *option : OPTIONS) {
    ClaudeSim t = m;
    const int matches = t.stats().matches;
    if (step(t, option[0], swapped) || step(t, option[1], swapped)) {
      continue;
    }
    if (t.cursorY() == ty + (t.shifts() - m.shifts()) && std::abs(t.cursorX() - tx) <= 1) {
      continue;
    }
    if (step(t, AI_SWAP, swapped) || !swapped || step(t, AI_WAIT, swapped)) {
      continue;
    }
    // The next planning tick, then the same swap the other way round.
    ClaudeSim u = t;
    if (step(u, AI_WAIT, swapped) || step(u, AI_SWAP, swapped) || !swapped
        || step(u, AI_WAIT, swapped) || step(u, AI_WAIT, swapped)) {
      continue;
    }
    if (u.stats().matches != matches) {
      continue;
    }
    actions.push_back(option[0]);
    actions.push_back(option[1]);
    actions.push_back(AI_SWAP);
    actions.push_back(AI_WAIT);
    m = t;
    return true;
  }
  return false;
}

bool ClaudeFableStrategy::play(const ClaudeSim &base, Move &move, bool leaf) const {
  ClaudeSim &m = move.end;
  m = base;
  m.tick();   // the planning tick: no input
  move.lost = false;
  int swaps = 0;
  int row = move.y;
  int shifts = m.shifts();
  bool swapped;
  for (size_t i = 0; i < move.actions.size(); ++i) {
    if (!m.running()) {
      break;
    }
    if (m.shifts() != shifts) {
      row += m.shifts() - shifts;
      shifts = m.shifts();
    }
    const AIStrategyAction a = move.actions[i];
    const bool core = (int) i >= move.core && a == AI_SWAP;
    if (core && move.kind == MOVE_DRAG && swaps > 0) {
      const ClaudeSim::Cell &c = m.cell(row, move.x + move.dir * swaps);
      if (c.type != ClaudeSim::BLOCK || c.color != move.color || c.state != NORMAL || c.falling) {
        return false;
      }
    }
    if (step(m, a, swapped)) {
      move.lost = true;
    }
    if (core) {
      if (!swapped) {
        return false;
      }
      ++swaps;
    }
  }
  if (m.dead()) {
    return true;
  }
  if (leaf) {
    // A second move: only what it sets off matters.
  } else if (move.kind == MOVE_STALL) {
    if (!continuable(m)) {
      move.lost = true;
    }
  } else if (!move.lost) {
    // Something set off (an explosion, a fall) keeps the board busy by
    // itself; a quiet rearrangement needs the cursor parked afterwards.
    ClaudeSim t = m;
    bool busy = true;
    for (int i = 0; i < 8 && busy; ++i) {
      busy = !step(t, AI_WAIT, swapped);
    }
    if (!busy && !park(m, move.actions, move.x + (move.kind == MOVE_DRAG ? move.dir * move.len : 0),
                       row)) {
      move.lost = true;
    }
  }
  rollout(m);
  return true;
}

double ClaudeFableStrategy::evaluate(const ClaudeSim &m) const {
  if (m.dead()) {
    return DEAD;
  }
  const ClaudeSim::Stats &s = m.stats();
  double v = _p.link * s.links;
  v += _p.clearCell * s.garbageCleared;
  v += _p.comboCell * s.comboCells;
  v -= _p.material * s.blocksCleared;

  int garbageCells = 0;
  int blocks = 0;
  int overflow = 0;   // cells on the top row and above it
  for (int row = 0; row < ClaudeSim::H; ++row) {
    for (int col = 0; col < W; ++col) {
      const ClaudeSim::Cell &c = m.cell(row, col);
      garbageCells += c.type == ClaudeSim::GARBAGE;
      blocks += c.type == ClaudeSim::BLOCK && c.state != EXPLODING && c.state != MATCHED;
      overflow += row >= Board::TOP_ROW && c.type != ClaudeSim::AIR;
    }
  }
  // Rows of garbage that were let in. A little is material; a lot buries
  // the board.
  v -= _p.breach * (_pendingRows - pendingRows(m));
  v -= _p.garbage * garbageCells;
  // While anything is on the top row the stack cannot be raised and the
  // board cannot be left idle; getting it clear is what brings material.
  v -= _p.overflow * overflow;
  // Keep enough blocks to dig out with: whatever the opponent has queued
  // lands the moment the chain is let go.
  if (blocks < _reserve) {
    v -= _p.low * (_reserve - blocks);
  }

  Grid g;
  gridFromSim(m, g);
  int triggers = 0;
  const double pot = potential(g, triggers);
  v += _p.pot * std::min(pot, 2.5);
  v += _p.option * std::min(triggers, 5);
  return v;
}

// What a move costs by itself: time, and every swap a little, so that
// nothing is done for nothing. Standing still is free.
double ClaudeFableStrategy::cost(const Move &move) const {
  if (move.kind == MOVE_STALL) {
    return 0;
  }
  return _p.tick * move.actions.size()
      + _p.effort * std::count(move.actions.begin(), move.actions.end(), AI_SWAP);
}

// ---------------------------------------------------------------------------
// moves
// ---------------------------------------------------------------------------

void ClaudeFableStrategy::buildMoves(const ClaudeSim &base, const Travel &travel,
                                     std::vector<Move> &moves) const {
  Move m;
  m.value = m.total = 0;
  m.lost = false;
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x + 1 < W; ++x) {
      if (!base.canSwap(x, y)) {
        continue;
      }
      const ClaudeSim::Cell &a = base.cell(y, x);
      const ClaudeSim::Cell &b = base.cell(y, x + 1);
      if (a.type == b.type && a.color == b.color) {
        continue;
      }
      m.kind = MOVE_SWAP;
      m.x = x;
      m.y = y;
      m.dir = 0;
      m.len = 1;
      m.color = -1;
      m.actions.clear();
      if (!travel.path(x, y, m.actions)) {
        continue;
      }
      m.core = (int) m.actions.size();
      m.actions.push_back(AI_SWAP);
      moves.push_back(m);
    }
  }

  for (int y = 0; y < ROWS && _p.maxDrag >= 2; ++y) {
    for (int x = 0; x < W; ++x) {
      const ClaudeSim::Cell &c = base.cell(y, x);
      if (c.type != ClaudeSim::BLOCK || c.state != NORMAL || c.falling) {
        continue;
      }
      for (int dir = -1; dir <= 1; dir += 2) {
        for (int len = 2; len <= _p.maxDrag; ++len) {
          const int last = x + dir * len;
          if (last < 0 || last >= W) {
            break;
          }
          bool ok = true;
          for (int k = 1; k <= len && ok; ++k) {
            const int col = x + dir * k;
            const ClaudeSim::Cell &t = base.cell(y, col);
            if (t.type == ClaudeSim::GARBAGE || t.state != NORMAL
                || (t.type == ClaudeSim::AIR && base.cell(y + 1, col).type == ClaudeSim::BLOCK)) {
              ok = false;
            }
            if (k < len && y > 0 && base.cell(y - 1, col).type == ClaudeSim::AIR) {
              ok = false;
            }
          }
          if (!ok) {
            break;
          }
          m.kind = MOVE_DRAG;
          m.x = x;
          m.y = y;
          m.dir = dir;
          m.len = len;
          m.color = c.color;
          m.actions.clear();
          if (!travel.path(dir > 0 ? x : x - 1, y, m.actions)) {
            continue;
          }
          m.core = (int) m.actions.size();
          m.actions.push_back(AI_SWAP);
          for (int k = 1; k < len; ++k) {
            // A swap takes SWAP_DELAY ticks; the block can go on after that.
            m.actions.push_back(dir > 0 ? AI_RIGHT : AI_LEFT);
            m.actions.push_back(AI_WAIT);
            m.actions.push_back(AI_SWAP);
          }
          moves.push_back(m);
        }
      }
    }
  }
}

// Doing nothing, safely: wait while the board is busy by itself, otherwise
// walk to the nearest pair that can be swapped without consequences and
// swap it.
bool ClaudeFableStrategy::stallMove(const Travel &travel, int free, int wait, Move &move) const {
  move.kind = MOVE_STALL;
  move.x = move.y = move.dir = move.len = 0;
  move.color = -1;
  move.core = 0;
  move.value = move.total = 0;
  move.lost = false;
  move.actions.clear();
  if (free >= Travel::FMAX) {
    // The board is busy by itself (or nothing needs holding): look again in
    // a few ticks. "free" says the next FMAX ticks are fine either way.
    move.actions.assign(std::max(1, std::min(wait, Travel::FMAX - 2)), AI_WAIT);
    return true;
  }
  int bestCost = NEVER;
  int bx = -1, by = -1, bf = 0;
  for (int pass = 0; pass < 2 && bestCost >= NEVER; ++pass) {
    for (int y = 0; y < ROWS; ++y) {
      for (int x = 0; x + 1 < W; ++x) {
        const int kind = travel.hop[y][x];
        if (kind == 0 || (pass == 0 && kind == 3)) {
          continue;
        }
        int f;
        const int cost = travel.best(x, y, f) + (kind == 1 ? 0 : 8);
        if (cost < bestCost) {
          bestCost = cost;
          bx = x;
          by = y;
          bf = f;
        }
      }
    }
  }
  if (bestCost >= NEVER) {
    // Nothing reachable in time: swap whatever is under the cursor.
    move.actions.push_back(AI_SWAP);
    move.actions.push_back(AI_WAIT);
    return true;
  }
  travel.path(bx, by, move.actions);
  if (bf >= 3) {
    // Early: wait there, leaving a tick for the next plan and one to swap.
    move.actions.insert(move.actions.end(), std::max(1, std::min(bf - 2, wait)), AI_WAIT);
  } else {
    move.actions.push_back(AI_SWAP);
    move.actions.push_back(AI_WAIT);
  }
  return true;
}

// Ends the held chain with the top row full: one idle tick (which is all a
// chain needs to end and send its garbage) and straight back to swapping.
// It costs one tick of the top-out grace timer, so there has to be plenty.
bool ClaudeFableStrategy::blip(const ClaudeSim &base, int activeTicks,
                               std::vector<AIStrategyAction> &actions) const {
  const int limit = (Board::STACK_RAISE_STEPS * base.stackRaiseTicks()) / 2;
  if (activeTicks < 1 || limit - base.graceTimer() < 40) {
    return false;
  }
  actions.assign(activeTicks, AI_WAIT);
  actions.push_back(AI_SWAP);
  actions.push_back(AI_WAIT);
  ClaudeSim m = base;
  m.tick();
  bool swapped = true;
  for (AIStrategyAction a : actions) {
    if (a == AI_SWAP) {
      swapped = m.swapAtCursor();
      m.tick();
    } else {
      m.step(a);
    }
  }
  if (m.dead() || !swapped || m.chainCounter() != 1 || m.graceTimer() > base.graceTimer() + 2) {
    actions.clear();
    return false;
  }
  return true;
}

// Raises the stack one more row when it already reaches row 10. The row
// shift then puts a block on the top row, where it must not stay: it is
// swapped off into the lower column next to it on the very next tick, before
// the grace timer can run. The cursor waits on that block (it moves up with
// the stack), and the whole sequence is checked on the model first.
bool ClaudeFableStrategy::raiseAtBrim(const ClaudeSim &base,
                                        std::vector<AIStrategyAction> &actions) const {
  ClaudeSim next = base;
  next.tick();
  if (next.shifts() != 0 || !next.running()) {
    return false;
  }
  int order[W - 1];
  for (int i = 0; i + 1 < W; ++i) {
    order[i] = i;
  }
  const int cx = next.cursorX();
  std::sort(order, order + W - 1, [cx](int a, int b) {
    return std::abs(a - cx) < std::abs(b - cx);
  });
  for (int x : order) {
    const bool left = resting(next, BRIM_ROW, x);
    const bool right = resting(next, BRIM_ROW, x + 1);
    const int air = left ? x + 1 : x;
    if (left == right || next.cell(BRIM_ROW, air).type != ClaudeSim::AIR
        || next.cell(BRIM_ROW + 1, air).type != ClaudeSim::AIR
        || next.cell(BRIM_ROW + 1, left ? x : x + 1).type != ClaudeSim::AIR) {
      continue;
    }
    std::vector<AIStrategyAction> list;
    for (int y = next.cursorY(); y < BRIM_ROW; ++y) {
      list.push_back(AI_UP);
    }
    for (int y = next.cursorY(); y > BRIM_ROW; --y) {
      list.push_back(AI_DOWN);
    }
    for (int k = cx; k < x; ++k) {
      list.push_back(AI_RIGHT);
    }
    for (int k = cx; k > x; --k) {
      list.push_back(AI_LEFT);
    }
    list.push_back(AI_RAISE);
    ClaudeSim m = next;
    bool swapped;
    bool bad = false;
    for (AIStrategyAction a : list) {
      bad = bad || step(m, a, swapped);
    }
    for (int i = 0; i < 40 && m.shifts() == 0 && !bad; ++i) {
      list.push_back(AI_WAIT);
      bad = step(m, AI_WAIT, swapped);
    }
    if (bad || m.shifts() != 1) {
      continue;
    }
    list.push_back(AI_SWAP);
    if (step(m, AI_SWAP, swapped) || !swapped || !continuable(m)) {
      continue;
    }
    rollout(m);
    // It has to end with the top row clear again.
    bool clear = m.running();
    for (int col = 0; col < W && clear; ++col) {
      clear = m.cell(Board::TOP_ROW, col).type == ClaudeSim::AIR;
    }
    if (!clear) {
      continue;
    }
    actions = list;
    return true;
  }
  return false;
}

// Last line of defence: replays the chosen actions on the model, and if
// one of them would let the grace timer run (or a held chain end), cuts the
// list there and swaps instead when that helps.
void ClaudeFableStrategy::secure(const ClaudeSim &base, std::vector<AIStrategyAction> &actions) const {
  ClaudeSim m = base;
  m.tick();
  bool swapped;
  for (size_t i = 0; i <= actions.size(); ++i) {
    // One tick past the end: the next planning tick, with no input.
    const AIStrategyAction a = i < actions.size() ? actions[i] : AI_WAIT;
    ClaudeSim t = m;
    if (!step(t, a, swapped)) {
      m = t;
      continue;
    }
    ClaudeSim u = m;
    if (a == AI_SWAP || step(u, AI_SWAP, swapped) || !swapped) {
      return;   // nothing better to offer
    }
    actions.resize(i);
    actions.push_back(AI_SWAP);
    actions.push_back(AI_WAIT);
    if (debugEnabled()) {
      printf("fable: t=%u secured the plan at action %d\n", base.ticksRun(), (int) i);
    }
    return;
  }
}

// ---------------------------------------------------------------------------
// plan
// ---------------------------------------------------------------------------

std::vector<AIStrategyAction> ClaudeFableStrategy::plan(const Board &board) {
  std::vector<AIStrategyAction> actions;
  if (board.getState() != Board::RUNNING) {
    return actions;
  }
  const Uint32 started = SDL_GetTicks();
  ++_plans;

  ClaudeSim base;
  base.import(board);
  base.resetStats();

  const int chain = base.chainCounter();
  const unsigned now = board.getTicksRun();
  if (chain > _lastChain) {
    _lastLinkTick = now;
    _stuck = false;
  }
  if (board.getScore() != _lastScore || now < _lastProgressTick) {
    _lastScore = board.getScore();
    _lastProgressTick = now;
  }
  const bool bored = now - _lastProgressTick > (unsigned) _p.bored;
  if (chain == 1) {
    if (_lastChain > 1 && !_releasing) {
      ++_lostChains;
      if (debugEnabled()) {
        printf("fable: t=%u lost a chain of %d (%ld so far)\n", now, _lastChain, _lostChains);
      }
    } else if (_lastChain > 1 && debugEnabled()) {
      printf("fable: t=%u released a chain of %d\n", now, _lastChain);
    }
    _releasing = false;
  }
  _lastChain = chain;

  // What happens if nothing is done: when does the board go idle, and when
  // would the grace timer start to run?
  int firstIdle = NEVER;
  int firstGrace = NEVER;
  {
    ClaudeSim m = base;
    int limit = 800;
    for (int i = 0; i < limit; ++i) {
      const int grace = m.graceTimer();
      m.tick();
      if (!m.activeFlag() && firstIdle == NEVER) {
        firstIdle = i;
        limit = std::min(limit, i + 48);
      }
      if (m.dead() || m.graceTimer() > grace) {
        firstGrace = i;
        break;
      }
    }
  }
  const bool quiet = firstIdle <= Board::SWAP_DELAY;   // nothing but our own swap is active
  const bool danger = firstGrace != NEVER;

  _holdChain = _p.hold != 0 && !_releasing;

  // Let the chain go?
  if (_holdChain && chain > 1 && quiet
      && (chain >= _p.target || _stuck || now - _lastLinkTick > (unsigned) _p.patience)) {
    if (firstGrace > firstIdle) {
      _releasing = true;
      _holdChain = false;
    } else if (blip(base, firstIdle, actions)) {
      _releasing = true;
      _stuck = false;
      return actions;
    }
  }
  if (_releasing && chain > 1 && firstGrace > 1) {
    actions.push_back(AI_WAIT);
    return actions;
  }

  _pendingRows = pendingRows(base);
  _reserve = std::min(_p.reserve + 2 * _pendingRows, 36);
  const int bad = (_holdChain && chain > 1) ? std::min(firstIdle, firstGrace) : firstGrace;
  const int free = bad == NEVER ? (int) Travel::FMAX : std::max(0, std::min(bad - 1, (int) Travel::FMAX));
  // New material: raise the stack while nothing is going on.
  bool restock = false;
  if (chain == 1 && quiet) {
    int top = -1;
    int blocks = 0;
    for (int row = ClaudeSim::H - 1; row >= 0; --row) {
      for (int col = 0; col < W; ++col) {
        const ClaudeSim::Cell &c = base.cell(row, col);
        if (c.type != ClaudeSim::AIR && top < 0) {
          top = row;
        }
        blocks += c.type == ClaudeSim::BLOCK;
      }
    }
    if (top < BRIM_ROW && !danger && firstIdle == 0) {
      if (base.raiseForced()) {
        actions.push_back(AI_WAIT);
        return actions;
      }
      if (top + pendingRows(base) < _p.raiseTop) {
        actions.push_back(AI_RAISE);
        return actions;
      }
    }
    restock = top == BRIM_ROW && blocks < _p.stock;
  }
  if (chain == 1 && quiet && (restock || bored)) {
    // Also when the search found nothing to do: more blocks, more options.
    if (raiseAtBrim(base, actions)) {
      if (debugEnabled()) {
        printf("fable: t=%u raise at the brim\n", now);
      }
      return actions;
    }
  }

  // Moves are made from the board as it is after the planning tick: a swap
  // of ours that is still under way has finished by then.
  ClaudeSim next = base;
  next.tick();
  Travel travel;
  travel.build(next, free);

  std::vector<Move> moves;
  moves.reserve(160);
  buildMoves(next, travel, moves);
  {
    Move stall;
    // While a chain is held time costs nothing, so a busy board is only
    // looked at now and then; an idle one is raised by the plan above.
    stallMove(travel, free, firstIdle > Travel::FMAX ? _p.busyWait : 1, stall);
    moves.push_back(stall);
  }
  // Nearest first, the stall move before everything: if the budget runs
  // out it is the far moves that go unsearched.
  std::stable_sort(moves.begin(), moves.end(), [](const Move &a, const Move &b) {
    if ((a.kind == MOVE_STALL) != (b.kind == MOVE_STALL)) {
      return a.kind == MOVE_STALL;
    }
    return a.actions.size() < b.actions.size();
  });

  long simulated = 0;
  size_t searched = 0;
  for (; searched < moves.size() && simulated < _p.budget; ++searched) {
    Move &mv = moves[searched];
    if (!play(base, mv, false)) {
      mv.value = mv.total = DEAD * 2;
      continue;
    }
    simulated += mv.end.simTicks();
    mv.value = evaluate(mv.end) - cost(mv) + (mv.lost ? LOST : 0.0);
    mv.total = mv.value;
  }
  moves.resize(searched);
  std::stable_sort(moves.begin(), moves.end(), [](const Move &a, const Move &b) {
    return a.value > b.value;
  });

  // A second move on top of the best few: rearrangements that pay off one
  // move later. Only what the second move sets off is looked at.
  if (_p.width > 0 && firstIdle <= Travel::FMAX) {
    std::vector<Move> seconds;
    Travel onward;
    for (int i = 0; i < _p.width && i < (int) moves.size() && simulated < _p.budget; ++i) {
      Move &first = moves[i];
      if (first.kind == MOVE_STALL || first.lost || !first.end.running()) {
        continue;
      }
      ClaudeSim from = first.end;
      from.tick();
      onward.build(from, Travel::FMAX);
      seconds.clear();
      buildMoves(from, onward, seconds);
      double best = first.value;
      for (Move &mv : seconds) {
        if (simulated >= _p.budget) {
          break;
        }
        const int before = first.end.simTicks();
        if (!play(first.end, mv, true)) {
          continue;
        }
        simulated += mv.end.simTicks() - before;
        best = std::max(best, evaluate(mv.end) - cost(first) - cost(mv));
      }
      first.total = first.value + _p.follow * (best - first.value);
    }
    std::stable_sort(moves.begin(), moves.end(), [](const Move &a, const Move &b) {
      return a.total > b.total;
    });
  }
  // A move has to be worth more than standing still by a margin, or two
  // equally good positions get swapped back and forth forever.
  size_t pick = 0;
  for (size_t i = 0; i < moves.size(); ++i) {
    if (moves[i].kind == MOVE_STALL) {
      if (moves[pick].kind != MOVE_STALL && moves[pick].total < moves[i].total + _p.margin) {
        pick = i;
      }
      break;
    }
  }
  // Nothing cleared for a long time and nothing in sight: make the best
  // match there is. It changes the board, which is what the search needs,
  // and otherwise the game can stand still forever (holding is safe, so
  // nothing else forces a move).
  {
    const ClaudeSim::Stats &st = moves[pick].end.stats();
    if (bored && st.blocksCleared == 0 && st.links == 0) {
      int forced = -1;
      for (size_t i = 0; i < moves.size(); ++i) {
        const Move &mv = moves[i];
        if (mv.kind != MOVE_STALL && !mv.lost && !mv.end.dead() && mv.end.stats().blocksCleared > 0
            && (forced < 0 || mv.total > moves[forced].total)) {
          forced = (int) i;
        }
      }
      if (forced >= 0) {
        pick = (size_t) forced;
      }
    }
  }
  if (moves[pick].kind == MOVE_STALL && chain == 1 && firstIdle == 0 && free >= Travel::FMAX) {
    // An idle board with nothing to do: no need to look again at once.
    moves[pick].actions.insert(moves[pick].actions.end(), 4, AI_WAIT);
  }
  const Move &chosen = moves[pick];
  _stuck = chosen.kind == MOVE_STALL && quiet && chain > 1;
  actions = chosen.actions;
  if (actions.empty()) {
    actions.push_back(AI_WAIT);
  }
  secure(base, actions);
  if (debugEnabled()) {
    describe(chosen, base, free, SDL_GetTicks() - started);
  }
  return actions;
}

void ClaudeFableStrategy::describe(const Move &move, const ClaudeSim &base, int free,
                                   unsigned ms) const {
  static const char *kinds[] = {"swap", "drag", "stall"};
  static const char *names = "URDLX^.";
  const ClaudeSim::Stats &s = move.end.stats();
  std::string acts;
  for (AIStrategyAction a : move.actions) {
    acts += names[a];
  }
  printf("fable: t=%u chain=%d free=%d %s(%d,%d%s%d) v=%.1f tot=%.1f links=%d clr=%d gclr=%d%s%s ms=%u [%s]\n",
         base.ticksRun(), base.chainCounter(), free, kinds[move.kind], move.x, move.y,
         move.dir > 0 ? ">" : (move.dir < 0 ? "<" : ""), move.len, move.value, move.total, s.links,
         s.blocksCleared, s.garbageCleared, move.lost ? " LOST" : "", _reserve > _p.reserve ? " pending" : "",
         ms, acts.c_str());
  fflush(stdout);
}
