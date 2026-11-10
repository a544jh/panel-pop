/*
 * ClaudeFableStrategy.h
 *
 * The "claudefable" AI: built to win and to never top out. It plays on the
 * same exact board model as "claude" (ClaudeSim) but is organised around
 * three rules of Board that decide everything:
 *
 *  - the chain counter only resets on a tick that starts with nothing
 *    active, and a block that is being swapped counts as active;
 *  - queued garbage only drops, the stack only rises and the top-out grace
 *    timer only runs on such an idle tick;
 *  - a chain link is any match made by a block that fell out of a clear,
 *    however long ago the previous link was.
 *
 * So ClaudeFable never lets the board go idle by accident. Between matches
 * it parks the cursor on a pair of blocks it can swap back and forth without
 * consequences and keeps swapping ("holding"), which freezes the chain
 * counter, the incoming garbage and the stack. Every small chain it then
 * sets off adds its links to the one long chain that is being held, and when
 * it runs out of material it lets go: the chain ends, the opponent gets up
 * to twelve rows at once, ClaudeFable raises its stack for new material and
 * starts over. With blocks on the top row it simply keeps holding, which is
 * why it does not die.
 *
 * See AGENTS.md ("ClaudeFable") for the details and the measurements.
 */

#ifndef AI_CLAUDEFABLESTRATEGY_H_
#define AI_CLAUDEFABLESTRATEGY_H_

#include <string>
#include <vector>

#include "AIStrategy.h"
#include "ClaudeSim.h"

class ClaudeFableStrategy : public AIStrategy {
 public:
  ClaudeFableStrategy();

  const std::string &name() const override;
  const std::string &key() const override;
  void setOptions(const std::string &options) override;
  std::vector<AIStrategyAction> plan(const Board &board) override;
  void reset() override;

  // Tuning, settable as "claudefable,target=8" (see setOptions()).
  struct Params {
    int hold;          // 0 = never hold a chain (plays like a plain searcher)
    int target;        // let the chain go once it is this long
    int patience;      // ... or after this many ticks without a new link
    int bored;         // ticks without clearing anything before a match is forced
    int maxDrag;       // longest drag considered (columns)
    int raiseTop;      // raise the stack until its top is on this row
    int width;         // moves that get a second move searched on top
    int budget;        // simulated ticks per plan (deterministic)
    int busyWait;      // ticks between looks while the board is busy
    int reserve;       // blocks to keep for digging out of garbage
    double link;       // value of one chain link
    double pot;        // value of a link one more move would start
    double option;     // value per available match
    double material;   // cost per block cleared (material is finite)
    double overflow;   // cost per cell on the top row or above it
    double clearCell;  // value per garbage cell turned into blocks
    double comboCell;  // value per garbage cell a combo sends
    double garbage;    // cost per garbage cell left on the board
    double tick;       // cost per tick a move takes
    double follow;     // weight of the second move's improvement
    double low;        // cost per block missing from the reserve
    double breach;     // cost per row of garbage let onto the board
    int stock;         // raise the stack until there are this many blocks
    double margin;     // a move must beat standing still by this much
    double effort;     // cost per swap
  };

 private:
  enum MoveKind {
    MOVE_SWAP, MOVE_DRAG, MOVE_STALL
  };

  struct Move {
    MoveKind kind;
    int x, y;        // block (drag) or left swap cell
    int dir, len;    // drag direction and length in columns
    int color;       // dragged block colour, checked at every step
    int core;        // index in actions of the first swap of the move itself
    std::vector<AIStrategyAction> actions;
    ClaudeSim end;   // after the actions and the rollout
    double value;
    double total;    // with the follow-up search
    bool lost;       // would let the chain end or the grace timer run
  };

  // Shortest cursor paths that keep the board active, see buildTravel().
  struct Travel;

  Params _p;
  bool _holdChain;       // this plan must not let the chain end
  bool _releasing;       // letting the held chain end on purpose
  bool _stuck;           // the last plan found nothing better than holding
  int _lastScore;        // the score moves whenever something is cleared
  unsigned _lastProgressTick;
  int _reserve;          // blocks to keep, this plan
  int _pendingRows;      // rows of garbage queued when this plan started
  int _lastChain;
  unsigned _lastLinkTick;
  long _plans;
  long _lostChains;

  bool step(ClaudeSim &m, AIStrategyAction a, bool &swapped) const;
  bool play(const ClaudeSim &base, Move &move, bool leaf) const;
  bool park(ClaudeSim &m, std::vector<AIStrategyAction> &actions, int tx, int ty) const;
  bool continuable(const ClaudeSim &m) const;
  void rollout(ClaudeSim &m) const;
  double evaluate(const ClaudeSim &m) const;
  double cost(const Move &move) const;
  void secure(const ClaudeSim &base, std::vector<AIStrategyAction> &actions) const;
  bool raiseAtBrim(const ClaudeSim &base, std::vector<AIStrategyAction> &actions) const;
  void buildMoves(const ClaudeSim &base, const Travel &travel, std::vector<Move> &moves) const;
  bool stallMove(const Travel &travel, int free, int wait, Move &move) const;
  bool blip(const ClaudeSim &base, int activeTicks, std::vector<AIStrategyAction> &actions) const;
  void describe(const Move &move, const ClaudeSim &base, int free, unsigned ms) const;
};

#endif /* AI_CLAUDEFABLESTRATEGY_H_ */
