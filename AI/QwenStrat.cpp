/*
 * QwenStrat.cpp
 */

#include "QwenStrat.h"

#include <SDL2/SDL_timer.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "../Game/Board.h"

namespace {

// Same seed for every candidate, so the colours the model has to invent
// (buffer rows, garbage interiors) are the same in every line of play.
const uint32_t RNG_BASE = 0x9e3779b9u;
const uint32_t RNG_FOLLOWUP = 0x85ebca6bu;

const double DEATH = -40000.0;

// Rows of garbage below the top row that count as "close" (see garbageNearTop).
const int GARBAGE_NEAR_ROWS = 3;

// Move markers for the non-swap candidates.
const int PASS = -1;
const int RAISE = -2;

} // namespace

QwenStrat::QwenStrat() :
    _dumped(false),
    _nextPlanTick(0) {
  // The search is anytime: it keeps adding second-ply lookahead until the
  // budget runs out, so a larger budget only means a deeper look at the
  // best candidates.
  _t.simTicks = 420;
  _t.followupTicks = 200;
  _t.lookaheadCandidates = 6;
  _t.budgetMs = 45;
  _t.followupWeight = 0.65;
  _t.travelCost = 1.2;
  // Ask again as soon as the queued actions are done: the AI loses games by
  // reacting too slowly, not by choosing badly (see AGENTS.md).
  _t.replanTicks = 1;
  _t.replanActiveTicks = 6;

  _w.score = 1.0;
  _w.garbage = 45.0;
  _w.chainLevel = 25.0;
  _w.pendingScore = 12.0;
  _w.height = 6.0;
  _w.heightSquare = 0.45;
  _w.holes = 16.0;
  _w.garbageRows = 14.0;
  _w.panicBlocks = 30.0;
  _w.topThreat = 220.0;
  _w.matchOptions = 40.0;
  _w.optionBlocks = 4.0;
  // Digging at garbage: a garbage cell a match sets off, garbage sitting close
  // to the top row, and the cost of raising the stack while it is covered.
  _w.dig = 40.0;
  _w.garbageNear = 10.0;
  _w.raiseGarbage = 400.0;
  _w.adjacency = 3.0;
  _w.columnSpread = 4.0;
  _w.raisePenalty = 0.0;
  _w.death = DEATH;

  const char *env = getenv("PANELPOP_QWEN_WEIGHTS");
  if (env != nullptr) {
    setOptions(env);
  }
}

// Weight tuning from a "key=value,key=value" string. PANELPOP_QWEN_WEIGHTS
// and the debug console ("strategy qwen,height=12") both end up here, which is
// how the weights are measured and tuned.
//
// Every knob by name, straight onto the member it sets. The names and the
// members used to be two parallel arrays walked with an index, which silently
// applied every weight to its neighbour two slots away ("height=12" moved
// "holes") and read two pointers past the end for the last two names -
// AddressSanitizer: global-buffer-overflow on "death=...". The tables live
// inside setOptions() because they name QwenStrat's private types.
void QwenStrat::setOptions(const std::string &options) {
  struct TuningName {
    const char *name;
    int Tuning::*member;
  };
  struct TuningDouble {
    const char *name;
    double Tuning::*member;
  };
  struct WeightName {
    const char *name;
    double Weights::*member;
  };
  static const TuningName tuningInts[] = {
      {"ticks", &Tuning::simTicks},
      {"followup", &Tuning::followupTicks},
      {"cand", &Tuning::lookaheadCandidates},
      {"replan", &Tuning::replanTicks},
      {"replanActive", &Tuning::replanActiveTicks},
  };
  static const TuningDouble tuningDoubles[] = {
      {"followupWeight", &Tuning::followupWeight},
      {"travel", &Tuning::travelCost},
  };
  static const WeightName weights[] = {
      {"score", &Weights::score},
      {"garbage", &Weights::garbage},
      {"chainLevel", &Weights::chainLevel},
      {"pendingScore", &Weights::pendingScore},
      {"height", &Weights::height},
      {"heightSquare", &Weights::heightSquare},
      {"holes", &Weights::holes},
      {"garbageRows", &Weights::garbageRows},
      {"panicBlocks", &Weights::panicBlocks},
      {"topThreat", &Weights::topThreat},
      {"matchOptions", &Weights::matchOptions},
      {"optionBlocks", &Weights::optionBlocks},
      {"dig", &Weights::dig},
      {"garbageNear", &Weights::garbageNear},
      {"raiseGarbage", &Weights::raiseGarbage},
      {"adjacency", &Weights::adjacency},
      {"columnSpread", &Weights::columnSpread},
      {"raisePenalty", &Weights::raisePenalty},
      {"death", &Weights::death},
  };

  const char *p = options.c_str();
  while (*p != 0) {
    char key[32];
    size_t n = 0;
    while (*p != 0 && *p != '=' && *p != ',' && *p != ' ') {
      if (n < sizeof(key) - 1) {
        key[n++] = *p;
      }
      ++p;
    }
    key[n] = 0;
    double value = 0;
    if (*p == '=') {
      ++p;
      char *end = nullptr;
      value = strtod(p, &end);
      p = end;
    }
    while (*p == ',' || *p == ' ') {
      ++p;
    }
    // The search shape first ("budget" is unsigned, so it is its own row),
    // then the evaluation weights.
    if (strcmp(key, "budget") == 0) {
      _t.budgetMs = static_cast<unsigned>(value);
      continue;
    }
    bool known = false;
    for (const TuningName &o : tuningInts) {
      if (strcmp(key, o.name) == 0) {
        _t.*o.member = static_cast<int>(value);
        known = true;
        break;
      }
    }
    if (known) {
      continue;
    }
    for (const TuningDouble &o : tuningDoubles) {
      if (strcmp(key, o.name) == 0) {
        _t.*o.member = value;
        known = true;
        break;
      }
    }
    if (known) {
      continue;
    }
    for (const WeightName &o : weights) {
      if (strcmp(key, o.name) == 0) {
        _w.*o.member = value;
        known = true;
        break;
      }
    }
    if (!known && getenv("PANELPOP_AI_DEBUG")) {
      printf("qwen: unknown option \"%s\" (ignored)\n", key);
      fflush(stdout);
    }
  }
}

// Prints the effective configuration under PANELPOP_AI_DEBUG, once per
// strategy: the console builds one, then hands it the options from the spec
// string ("strategy qwen,height=12 2"), so printing at construction or in
// setOptions() would show a line that is not the configuration in use.
void QwenStrat::dump() {
  if (_dumped || !getenv("PANELPOP_AI_DEBUG")) {
    return;
  }
  _dumped = true;
  printf("qwen: ticks=%d followup=%d cand=%d budget=%u replan=%d replanActive=%d "
         "followupWeight=%.2f travel=%.2f | score=%.1f garbage=%.1f chainLevel=%.1f "
         "pendingScore=%.1f height=%.1f heightSquare=%.2f holes=%.1f garbageRows=%.1f "
         "panicBlocks=%.1f topThreat=%.1f matchOptions=%.1f optionBlocks=%.1f dig=%.1f "
         "garbageNear=%.1f raiseGarbage=%.1f "
         "adjacency=%.1f columnSpread=%.1f raisePenalty=%.1f death=%.0f\n",
         _t.simTicks, _t.followupTicks, _t.lookaheadCandidates, _t.budgetMs,
         _t.replanTicks, _t.replanActiveTicks, _t.followupWeight, _t.travelCost,
         _w.score, _w.garbage, _w.chainLevel, _w.pendingScore, _w.height,
         _w.heightSquare, _w.holes, _w.garbageRows, _w.panicBlocks, _w.topThreat,
         _w.matchOptions, _w.optionBlocks, _w.dig, _w.garbageNear, _w.raiseGarbage,
         _w.adjacency, _w.columnSpread,
         _w.raisePenalty, _w.death);
  fflush(stdout);
}

void QwenStrat::reset() {
  _nextPlanTick = 0;
}

const std::string &QwenStrat::name() const {
  static const std::string kName = "QwenStrat";
  return kName;
}

const std::string &QwenStrat::key() const {
  static const std::string kKey = "qwen";
  return kKey;
}

// Length of the same-colour run of blocks through (row, col).
int QwenStrat::runSpan(const SimBoard &model, int row, int col, bool horizontal) {
  const SimBoard::Cell &c = model.cell(row, col);
  if (c.type != SimBoard::BLOCK) {
    return 0;
  }
  int n = 1;
  if (horizontal) {
    for (int i = col - 1; i >= 0 && model.cell(row, i).type == SimBoard::BLOCK
        && model.cell(row, i).color == c.color; --i) {
      ++n;
    }
    for (int i = col + 1; i < SimBoard::W && model.cell(row, i).type == SimBoard::BLOCK
        && model.cell(row, i).color == c.color; ++i) {
      ++n;
    }
  } else {
    for (int i = row - 1; i >= 0 && model.cell(i, col).type == SimBoard::BLOCK
        && model.cell(i, col).color == c.color; --i) {
      ++n;
    }
    for (int i = row + 1; i < SimBoard::H && model.cell(i, col).type == SimBoard::BLOCK
        && model.cell(i, col).color == c.color; ++i) {
      ++n;
    }
  }
  return n;
}

// Garbage cells orthogonally adjacent to the run of blocks through (row, col):
// exactly what a match there would set off. Board::triggerTile() only reacts
// for a garbage block still in state NORMAL, and any adjacent matched block
// triggers it - gray included - so that is the test used here. Each cell is
// counted once.
int QwenStrat::garbageAlongRun(const SimBoard &model, int row, int col, bool horizontal) {
  const uint8_t color = model.cell(row, col).color;
  bool seen[SimBoard::H][SimBoard::W] = {};
  int cells = 0;
  const int steps = horizontal ? SimBoard::W : SimBoard::H;
  int lo = horizontal ? col : row;
  int hi = lo;
  while (lo - 1 >= 0 && (horizontal ? model.cell(row, lo - 1) : model.cell(lo - 1, col)).type == SimBoard::BLOCK
      && (horizontal ? model.cell(row, lo - 1) : model.cell(lo - 1, col)).color == color) {
    --lo;
  }
  while (hi + 1 < steps && (horizontal ? model.cell(row, hi + 1) : model.cell(hi + 1, col)).type == SimBoard::BLOCK
      && (horizontal ? model.cell(row, hi + 1) : model.cell(hi + 1, col)).color == color) {
    ++hi;
  }
  for (int k = lo; k <= hi; ++k) {
    const int r = horizontal ? row : k;
    const int c = horizontal ? k : col;
    const int dr[4] = {1, -1, 0, 0};
    const int dc[4] = {0, 0, 1, -1};
    for (int n = 0; n < 4; ++n) {
      const int nr = r + dr[n];
      const int nc = c + dc[n];
      if (nr < 0 || nc < 0 || nr >= SimBoard::H || nc >= SimBoard::W || seen[nr][nc]) {
        continue;
      }
      const SimBoard::Cell &t = model.cell(nr, nc);
      if (t.type != SimBoard::GARBAGE || t.g < 0 || t.g >= model.numGarbage()
          || model.garbage(t.g).state != 0 /* GarbageBlockState::NORMAL */) {
        continue;
      }
      seen[nr][nc] = true;
      ++cells;
    }
  }
  return cells;
}

int QwenStrat::digCellsForSwap(const SimBoard &model, int x, int y) {
  if (!model.canSwap(x, y)) {
    return 0;
  }
  SimBoard m = model;
  const SimBoard::Cell a = m.cell(y, x);
  const SimBoard::Cell b = m.cell(y, x + 1);
  m.cell(y, x) = b;
  m.cell(y, x + 1) = a;
  int cells = 0;
  for (int k = 0; k < 2; ++k) {
    for (int horizontal = 0; horizontal < 2; ++horizontal) {
      if (runSpan(m, y, x + k, horizontal != 0) >= 3) {
        cells += garbageAlongRun(m, y, x + k, horizontal != 0);
      }
    }
  }
  return cells;
}

int QwenStrat::highestGarbageRow(const SimBoard &model) {
  int top = -1;
  for (int i = 0; i < model.numGarbage(); ++i) {
    top = std::max(top, static_cast<int>(model.garbage(i).y));
  }
  return top;
}

int QwenStrat::garbageNearTop(const SimBoard &model, int rows) {
  const int limit = Board::TOP_ROW - rows;
  int near = 0;
  for (int i = 0; i < model.numGarbage(); ++i) {
    const SimBoard::Garbage &g = model.garbage(i);
    const int over = std::min(static_cast<int>(g.y) - limit + 1, static_cast<int>(g.h));
    if (over > 0) {
      near += over;
    }
  }
  return near;
}

void QwenStrat::immediateMatches(const SimBoard &model, int &moves, int &blocks,
                                 int &digCells) {
  moves = 0;
  blocks = 0;
  digCells = 0;

  SimBoard m = model;
  for (int row = 0; row < SimBoard::H; ++row) {
    for (int col = 0; col < SimBoard::W - 1; ++col) {
      if (m.cell(row, col).type != SimBoard::BLOCK
          || m.cell(row, col + 1).type != SimBoard::BLOCK) {
        continue;
      }
      if (m.cell(row, col).color == m.cell(row, col + 1).color) {
        continue;
      }
      // exchange the two blocks and look for a run through either of them
      const SimBoard::Cell a = m.cell(row, col);
      const SimBoard::Cell b = m.cell(row, col + 1);
      m.cell(row, col) = b;
      m.cell(row, col + 1) = a;

      int best = 0;
      int digs = 0;
      for (int k = 0; k < 2; ++k) {
        for (int horizontal = 0; horizontal < 2; ++horizontal) {
          const int span = runSpan(m, row, col + k, horizontal != 0);
          best = std::max(best, span);
          if (span >= 3) {
            digs += garbageAlongRun(m, row, col + k, horizontal != 0);
          }
        }
      }

      if (best >= 3) {
        ++moves;
        blocks += best;
        digCells += digs;
      }

      m.cell(row, col) = a;
      m.cell(row, col + 1) = b;
    }
  }
}

double QwenStrat::evaluate(const SimBoard &m, const SimBoard &base, bool survival) const {
  if (m.state() == Board::GAME_OVER) {
    return _w.death;
  }

  // What the line of play achieved...
  double value = _w.score * (m.score() - base.score());
  value += _w.garbage * (m.garbageSent() - base.garbageSent());
  value += _w.chainLevel * (m.maxChain() - base.maxChain());
  // ...plus what it has going on right now: explosions that are about to
  // clear blocks, and chains that are still running when the horizon ends.
  value += _w.pendingScore * m.pendingExplosions();
  if (m.chainCounter() > 1) {
    value += _w.chainLevel * (m.chainCounter() - 1);
  }

  // ...and how much room is left to keep playing.
  int height = 0;
  int heightSquare = 0;
  int holes = 0;
  int highest = 0;
  int lowest = SimBoard::H;
  const int topOccupied = m.topRow();
  for (int col = 0; col < SimBoard::W; ++col) {
    int top = -1;
    for (int row = topOccupied; row >= 0; --row) {
      if (m.cell(row, col).type != SimBoard::AIR) {
        top = row;
        break;
      }
    }
    int air = 0;
    for (int row = 0; row <= top; ++row) {
      if (m.cell(row, col).type == SimBoard::AIR) {
        ++air;
      }
    }
    if (top >= 0) {
      height += top + 1;
      heightSquare += (top + 1) * (top + 1);
      holes += air;
      highest = std::max(highest, top);
      lowest = std::min(lowest, top);
    }
  }

  value -= _w.height * height;
  value -= _w.heightSquare * heightSquare;
  value -= _w.holes * holes;
  value -= _w.garbageRows * m.garbageRows();
  // Garbage close to the top is not the same problem as garbage low down: the
  // board dumps it in drops of three or four rows, and the rows that reach
  // row 9 or 10 are the ones that decide the game.
  const int nearTop = garbageNearTop(m, GARBAGE_NEAR_ROWS);
  value -= _w.garbageNear * nearTop;
  value -= _w.panicBlocks * m.blocksAbove(Board::PANIC_HEIGHT);
  if (lowest <= highest) {
    value -= _w.columnSpread * (highest - lowest);
  }

  // How close the stack is to the top row: the real killer in this game.
  int head = Board::TOP_ROW - m.highestOccupiedRow();
  if (head < 3) {
    value -= _w.topThreat * (3 - head);
  }
  if (m.graceTimer() > 0) {
    value -= _w.topThreat;
  }

  // Options: swaps that score right away, and pairs of the same colour that
  // are one move away from doing so. This is what keeps the AI clearing and
  // chain building instead of parking a pretty but dead stack.
  int optionMoves = 0;
  int optionBlocks = 0;
  int digCells = 0;
  immediateMatches(m, optionMoves, optionBlocks, digCells);
  value += _w.matchOptions * optionMoves;
  value += _w.optionBlocks * optionBlocks;
  // Garbage a match would set off right now. Without this the AI sits on such
  // a clear for hundreds of ticks: converting garbage into blocks changes
  // almost nothing in the height and hole counts, so the dig looks free and
  // loses to anything that scores points on the spot.
  value += _w.dig * digCells;

  int pairs = 0;
  for (int row = 0; row <= topOccupied; ++row) {
    for (int col = 0; col < SimBoard::W; ++col) {
      const SimBoard::Cell &c = m.cell(row, col);
      if (c.type != SimBoard::BLOCK) {
        continue;
      }
      if (col + 1 < SimBoard::W && m.cell(row, col + 1).type == SimBoard::BLOCK
          && m.cell(row, col + 1).color == c.color) {
        ++pairs;
      }
      if (row + 1 < SimBoard::H && m.cell(row + 1, col).type == SimBoard::BLOCK
          && m.cell(row + 1, col).color == c.color) {
        ++pairs;
      }
    }
  }
  value += _w.adjacency * pairs;

  if (survival) {
    // Digging out: everything that lowers the stack counts harder, the
    // opponent does not care about the garbage we would have sent.
    value -= 1.5 * _w.height * height;
    value -= 1.0 * _w.heightSquare * heightSquare;
    value -= 1.5 * _w.holes * holes;
    value -= 1.5 * _w.panicBlocks * m.blocksAbove(Board::PANIC_HEIGHT);
    value -= 1.0 * _w.topThreat * (3 - head > 0 ? 3 - head : 0);
    // The garbage is what is killing us, so getting it off the board outranks
    // everything the line cleared elsewhere. It used to be the other way
    // round: panic halved the value of having a match available at all, which
    // is precisely when the AI stopped clearing anything.
    value -= 1.5 * _w.garbageRows * m.garbageRows();
    value -= 1.5 * _w.garbageNear * nearTop;
    value += 1.0 * _w.dig * digCells;
  }

  return value;
}

double QwenStrat::playAndValue(SimBoard &model, const SimBoard &base, int ticks,
                               bool survival) const {
  for (int t = 0; t < ticks; ++t) {
    model.tick();
    if (model.state() == Board::GAME_OVER) {
      // Dying later is still better than dying early.
      return _w.death + 60.0 * t;
    }
  }
  return evaluate(model, base, survival);
}

int QwenStrat::generateMoves(const SimBoard &base, std::vector<Move> &moves,
                             bool survival, int cursorX, int cursorY) const {
  moves.clear();

  for (int y = 0; y <= Board::TOP_ROW; ++y) {
    for (int x = 0; x + 1 < SimBoard::W; ++x) {
      if (!base.canSwap(x, y)) {
        continue;
      }
      SimBoard model = base;
      model.setRng(RNG_BASE);
      model.startSwap(x, y);

      Move move;
      move.x = x;
      move.y = y;
      move.followUp = 0;
      move.dig = digCellsForSwap(base, x, y);
      move.value = playAndValue(model, base, _t.simTicks, survival);
      // All else equal, take the swap that is quickest to reach.
      move.value -= _t.travelCost * (std::abs(x - cursorX) + std::abs(y - cursorY));
      moves.push_back(move);
    }
  }

  // The D button: brings new blocks in (and one point) a bit earlier.
  {
    SimBoard model = base;
    model.setRng(RNG_BASE);
    model.forceRaise();
    Move move;
    move.x = RAISE;
    move.y = 0;
    move.followUp = 0;
    move.dig = 0;
    move.value = playAndValue(model, base, _t.simTicks, survival) - _w.raisePenalty;
    // The D button lifts everything, garbage included: under a garbage cover
    // it walks the hazard up to the top row instead of away from it, and the
    // new blocks land on top of the problem. Measured, this candidate was
    // picked for 40-55% of plans while garbage sat at rows 4-6.
    if (highestGarbageRow(base) >= Board::PANIC_HEIGHT) {
      move.value -= _w.raiseGarbage;
    }
    moves.push_back(move);
  }

  // Doing nothing, the baseline every swap has to beat.
  {
    SimBoard model = base;
    model.setRng(RNG_BASE);
    Move move;
    move.x = PASS;
    move.y = 0;
    move.followUp = 0;
    move.dig = 0;
    move.value = playAndValue(model, base, _t.simTicks, survival);
    moves.push_back(move);
  }

  std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b) {
    return a.value > b.value;
  });
  return (int) moves.size();
}

void QwenStrat::appendCursorPath(int fromX, int fromY, int toX, int toY,
                                 std::vector<AIStrategyAction> &out) {
  while (fromY > toY) {
    out.push_back(AI_DOWN);
    --fromY;
  }
  while (fromY < toY) {
    out.push_back(AI_UP);
    ++fromY;
  }
  while (fromX > toX) {
    out.push_back(AI_LEFT);
    --fromX;
  }
  while (fromX < toX) {
    out.push_back(AI_RIGHT);
    ++fromX;
  }
}

std::vector<AIStrategyAction> QwenStrat::plan(const Board &board) {
  dump();
  std::vector<AIStrategyAction> actions;

  if (board.getTicksRun() < _nextPlanTick) {
    return actions;
  }

  SimBoard base;
  base.import(board);
  if (base.state() != Board::RUNNING) {
    return actions;
  }
  // Re-plan at most every few ticks; while blocks are moving there is little
  // to decide and the search is not cheap.
  _nextPlanTick = board.getTicksRun()
      + (base.hasActiveBlocks() ? _t.replanActiveTicks : _t.replanTicks);

  // Danger, in the order it actually arrives: the game's own panic flag, the
  // stack at the warning row, garbage within reach of the top row, and the
  // top-out clock already running.
  const bool survival = board.isPanic()
      || base.highestOccupiedRow() >= Board::WARN_HEIGHT
      || highestGarbageRow(base) >= Board::PANIC_HEIGHT
      || base.graceTimer() > 0;

  const Uint32 searchStart = SDL_GetTicks();

  std::vector<Move> moves;
  if (generateMoves(base, moves, survival, board.getCursorX(), board.getCursorY()) == 0) {
    return actions;
  }

  // Second ply for the most promising swaps: this is what makes the AI play
  // setup moves that only pay off one cascade later.
  const Uint32 start = searchStart;
  auto refine = [&](size_t i) {
    SimBoard after = base;
    after.setRng(RNG_BASE);
    after.startSwap(moves[i].x, moves[i].y);
    if (playAndValue(after, base, _t.simTicks, survival) <= _w.death + 1) {
      return; // this line of play dies inside the horizon, no follow-up
    }

    SimBoard waiting = after;
    waiting.setRng(RNG_FOLLOWUP);
    const double passValue = playAndValue(waiting, after, _t.followupTicks, survival);
    double best = passValue;
    for (int y = 0; y <= Board::TOP_ROW; ++y) {
      for (int x = 0; x + 1 < SimBoard::W; ++x) {
        if (!after.canSwap(x, y)) {
          continue;
        }
        SimBoard next = after;
        next.setRng(RNG_FOLLOWUP);
        next.startSwap(x, y);
        best = std::max(best, playAndValue(next, after, _t.followupTicks, survival));
      }
    }
    // Only the improvement over simply waiting is worth anything: the
    // position itself is already counted in the first ply value.
    moves[i].value += _t.followupWeight * (best - passValue);
  };

  int refined = 0;
  size_t scanned = 0;
  for (size_t i = 0; i < moves.size() && refined < _t.lookaheadCandidates; ++i) {
    if (moves[i].x < 0) {
      break;
    }
    if (refined > 0 && SDL_GetTicks() - start > _t.budgetMs) {
      break;
    }
    ++refined;
    scanned = i + 1;
    refine(i);
  }

  // The dig is the move this prune is worst at seeing: the search keeps the
  // best `cand` candidates by first-ply value, and the first move of a dig
  // converts garbage into blocks without clearing much of anything, so it
  // ranks badly exactly when digging is what saves the game. Give the best
  // available dig its second ply regardless of rank.
  if (survival && refined > 0) {
    int bestDig = 0;
    size_t bestIdx = 0;
    for (size_t i = scanned; i < moves.size() && moves[i].x >= 0; ++i) {
      if (moves[i].dig > bestDig) {
        bestDig = moves[i].dig;
        bestIdx = i;
      }
    }
    if (bestDig > 0 && SDL_GetTicks() - start <= _t.budgetMs) {
      refine(bestIdx);
      ++refined;
    }
  }

  std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b) {
    return a.value > b.value;
  });

  if (getenv("PANELPOP_AI_DEBUG")) {
    printf("ai: ticks=%d candidates=%d refined=%d survival=%d ms=%u best=(%d,%d) "
           "dig=%d v=%.1f\n",
           (int) board.getTicksRun(), (int) moves.size(), refined, (int) survival,
           SDL_GetTicks() - searchStart, moves.front().x, moves.front().y,
           moves.front().dig, moves.front().value);
    fflush(stdout);
  }

  const Move &best = moves.front();
  if (best.x == PASS) {
    return actions;
  }
  if (best.x == RAISE) {
    actions.push_back(AI_RAISE);
    return actions;
  }

  appendCursorPath(board.getCursorX(), board.getCursorY(), best.x, best.y, actions);
  actions.push_back(AI_SWAP);
  return actions;
}
