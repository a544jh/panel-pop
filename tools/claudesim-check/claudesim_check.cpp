/*
 * claudesim_check.cpp - lockstep fidelity check of AI/ClaudeSim against the real
 * Game/Board (see AGENTS.md, "ClaudeSim").
 *
 *   cmake --build build --target claudesim-check
 *   ./build/claudesim-check [seed] [ticks] [window] [swap%]
 *
 * Drives a real Board with random input (cursor moves, swaps, raises) and
 * random incoming garbage, and every tick compares it field by field with
 *   1tick: a ClaudeSim imported from the board one tick earlier, and
 *   long:  a ClaudeSim imported once per window and fed the same input,
 *          until the model has had to invent a colour (new rows).
 * Prints the first differences and a per-field count; exits 1 on any.
 * Run it after changing anything in Board: ClaudeStrategy's skill chains rely
 * on the model being tick-exact.
 */
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

#include "../../Game/Board.h"
#include "../../AI/ClaudeSim.h"

// Board only calls its event handler for sound and effects.
BoardEventHandler::~BoardEventHandler() {}
void BoardEventHandler::endTick() {}
void BoardEventHandler::blockExplode(int, int, int, int, int) {}
void BoardEventHandler::cursorMove() {}
void BoardEventHandler::swap() {}
void BoardEventHandler::blockFall() {}
void BoardEventHandler::gbFall(bool) {}
void BoardEventHandler::combo(int, int, int, int) {}
void BoardEventHandler::chain(int, int, int, int) {}
void BoardEventHandler::chainEnd(int) {}

static std::map<std::string, int> diffs;
static int reports = 0;
static int maxReports = 10;

static bool compare(const Board &b, const ClaudeSim &m, int tick, const char *mode) {
  std::string what;
  char where[96] = "";
  for (int r = 0; r < Board::BOARD_HEIGHT && what.empty(); ++r) {
    for (int c = 0; c < Board::BOARD_WIDTH && what.empty(); ++c) {
      const Board::Tile &t = b.getTile(r, c);
      const ClaudeSim::Cell &s = m.cell(r, c);
      snprintf(where, sizeof(where), "(r%d,c%d)", r, c);
      if ((int) t.type != (int) s.type) what = "type";
      else if (t.chain != (bool) s.chain) what = "chainflag";
      else if ((int) t.b._state != (int) s.state) what = "state";
      else if (t.type == BLOCK && t.b._color != s.color) what = "color";
      else if (t.type == BLOCK && t.b._falling != (bool) s.falling) what = "falling";
      else if (t.type == BLOCK && t.b._gbFallChain != (bool) s.gbFall) what = "gbFall";
      else if (t.b._state == FLOATING && t.b._floatTimer != s.floatTimer) what = "floatTimer";
      else if ((t.b._state == SWAPPING_LEFT || t.b._state == SWAPPING_RIGHT)
               && t.b._swapTimer != s.swapTimer) what = "swapTimer";
      else if (t.type == BLOCK && t.b._state == EXPLODING
               && (t.b._explosionTimer != s.explTimer || t.b._explosionTicks != s.explTicks
                   || t.b._explosionAnimTicks != s.animTicks)) what = "explTimer";
    }
  }
  if (what.empty()) {
    snprintf(where, sizeof(where), "-");
    if (b.getCursorX() != m.cursorX() || b.getCursorY() != m.cursorY()) what = "cursor";
    else if (b.getScore() != m.score()) what = "score";
    else if (b.getChainCounter() != m.chainCounter()) what = "chainCounter";
    else if (b.getStackOffset() != m.stackOffset()) what = "stackOffset";
    else if (b.getStackRaiseTimer() != m.stackRaiseTimer()) what = "raiseTimer";
    else if (b.getStackRaiseTicks() != m.stackRaiseTicks()) what = "raiseTicks";
    else if (b.isStackRaiseForced() != m.raiseForced()) what = "forced";
    else if (b.getGraceTimer() != m.graceTimer()) what = "grace";
    else if (b.getState() != m.state()) what = "boardState";
    else if (b.hasActiveBlocks() != m.activeFlag()) what = "active";
    else if (b.isPanic() != m.panic()) what = "panic";
    else if (b.getTickMatched() != m.tickMatched()) what = "tickMatched";
    else if (b.isTickChainEnd() != m.tickChainEnd()) what = "tickChainEnd";
    else if (b.getLastChain() != m.lastChain()) what = "lastChain";
    else if ((int) b.getGarbageBlocks().size() != m.numGarbage()) what = "numGarbage";
    else if ((int) b.getGarbageQueue().size() != m.numSpawns()) what = "numSpawns";
    else {
      int i = 0;
      for (const GarbageBlock &g : b.getGarbageBlocks()) {
        const ClaudeSim::Garbage &s = m.garbage(i++);
        if (g.getX() != s.x || g.getY() != s.y || g.getW() != s.w || g.getH() != s.h
            || (int) g.getState() != s.state || g.getTransformationTimer() != s.transTimer
            || g.getTransformationTicks() != s.transTicks || g.isFalling() != (bool) s.falling
            || g.isInitialFall() != (bool) s.initialFall) {
          what = "garbage";
          break;
        }
      }
      i = 0;
      for (const Board::GarbageSpawn &g : b.getGarbageQueue()) {
        const ClaudeSim::Spawn &s = m.spawn(i++);
        if (g.spawnTimer != s.timer || g.size != s.size) {
          what = "spawn";
          break;
        }
      }
      for (int k = 0; k < 3 && what.empty(); ++k) {
        if (b.getGarbageSpawnPosition(k) != m.spawnPosition(k)) what = "spawnPos";
      }
    }
  }
  if (what.empty()) {
    return true;
  }
  diffs[std::string(mode) + ":" + what]++;
  if (reports < maxReports) {
    ++reports;
    printf("[%s] tick %d %s %s\n", mode, tick, what.c_str(), where);
  }
  return false;
}

int main(int argc, char **argv) {
  int seed = argc > 1 ? atoi(argv[1]) : 1;
  int ticks = argc > 2 ? atoi(argv[2]) : 20000;
  int window = argc > 3 ? atoi(argv[3]) : 200;
  int swapPct = argc > 4 ? atoi(argv[4]) : 30;
  srand(seed);
  alignas(BoardEventHandler) static char ehBuf[sizeof(BoardEventHandler)];
  BoardEventHandler *eh = reinterpret_cast<BoardEventHandler *>(ehBuf);

  Board b;
  b.setEventHandler(eh);
  for (int i = 0; i < 3; ++i) b.advanceCountdownState();
  b.tick();

  ClaudeSim longModel;
  int longStart = -1;
  bool longValid = false;
  int games = 0, longTicks = 0, chains = 0, maxChain = 0, gbCleared = 0;
  for (int t = 0; t < ticks; ++t) {
    if (b.getState() != Board::RUNNING) {
      b = b.reset();
      for (int i = 0; i < 3; ++i) b.advanceCountdownState();
      b.tick();
      ++games;
      longStart = -1;
      continue;
    }
    ClaudeSim one;
    one.import(b);
    if (longStart < 0 || t - longStart >= window || !longValid) {
      longModel.import(b);
      longStart = t;
      longValid = true;
    }
    int r = rand() % 100;
    AIStrategyAction a = AI_WAIT;
    if (r < 25) {
      a = (AIStrategyAction) (rand() % 4); // UP RIGHT DOWN LEFT
    } else if (r < 25 + swapPct) {
      a = AI_SWAP;
    } else if (r < 26 + swapPct && rand() % 3 == 0) {
      a = AI_RAISE;
    }
    switch (a) {
      case AI_UP: b.inputMoveCursor(UP); break;
      case AI_DOWN: b.inputMoveCursor(DOWN); break;
      case AI_LEFT: b.inputMoveCursor(LEFT); break;
      case AI_RIGHT: b.inputMoveCursor(RIGHT); break;
      case AI_SWAP: b.inputSwapBlocks(); break;
      case AI_RAISE: b.inputForceStackRaise(); break;
      default: break;
    }
    one.apply(a);
    longModel.apply(a);
    if (rand() % 500 == 0) {
      bool full = rand() % 2;
      int size = full ? 1 + rand() % 3 : 3 + rand() % 4;
      b.queueGarbage(full, size, GarbageBlockType::NORMAL);
      one.queueGarbage(full, size, GarbageBlockType::NORMAL);
      longModel.queueGarbage(full, size, GarbageBlockType::NORMAL);
    }
    int gbBefore = (int) b.getGarbageBlocks().size();
    b.tick();
    one.tick();
    longModel.tick();
    if (b.isTickChainEnd()) {
      ++chains;
      if (b.getLastChain() > maxChain) maxChain = b.getLastChain();
    }
    if ((int) b.getGarbageBlocks().size() < gbBefore) ++gbCleared;
    compare(b, one, t, "1tick");
    if (longValid && !longModel.inventedColors()) {
      ++longTicks;
      if (!compare(b, longModel, t, "long")) {
        longValid = false;
      }
    } else {
      longValid = false;
    }
  }
  printf("seed %d: %d ticks, %d game overs, %d chains (max %d), %d garbage removed, %d long-compared ticks\n",
         seed, ticks, games, chains, maxChain, gbCleared, longTicks);
  for (auto &kv : diffs) {
    printf("  %-24s %d\n", kv.first.c_str(), kv.second);
  }
  return diffs.empty() ? 0 : 1;
}
