/*
 * ProStrategy.h
 *
 * The "pro" AI: plays like a good Panel de Pon player. It builds chains on
 * purpose, extends running chains with skill-chain swaps timed against the
 * falling blocks, keeps a flat stack at a useful height (raising it when it
 * runs low), digs through garbage, and moves its cursor at a human pace
 * instead of one input per frame.
 *
 * How it decides (see AGENTS.md for the long version):
 *
 *  - Candidate moves are single swaps, drags (one block slid two or three
 *    columns), a stack raise and waiting. Each is played on ProSim, an
 *    exact model of the board, with the very action list the controller
 *    will play (cursor travel and pacing included), then the model runs on
 *    until the board settles and any running chain has ended. Timing is
 *    therefore real: a skill-chain swap is only found if the cursor can get
 *    there before the falling block lands.
 *  - A line of play is worth what it actually did (garbage sent by chains
 *    and combos, garbage cleared, survival) plus what the settled board
 *    promises: chain potential (the best chain one more move could set off,
 *    found by a fast timing-free resolver), height, flatness, garbage on
 *    the board and garbage about to drop.
 *  - The best few moves get a second move searched on top, so it plays
 *    setups whose payoff is two moves away.
 *  - Between plans it checks that the real board is exactly where the model
 *    said it would be; PANELPOP_PRO_VERIFY=1 reports any difference.
 *
 * Deterministic: the search budget counts simulated ticks, not wall clock.
 */

#ifndef AI_PROSTRATEGY_H_
#define AI_PROSTRATEGY_H_

#include <string>
#include <vector>

#include "AIStrategy.h"
#include "ProSim.h"

class ProStrategy : public AIStrategy {
 public:
  ProStrategy();

  const std::string &name() const override;
  const std::string &key() const override;
  void setOptions(const std::string &options) override;
  std::vector<AIStrategyAction> plan(const Board &board) override;
  void reset() override;

  // Tuning, settable as "pro,pace=3,depth=1" (see setOptions()).
  struct Params {
    int pace;          // ticks per input when calm
    int fastPace;      // ticks per input in a chain or in danger
    int maxDrag;       // longest drag considered (columns)
    int depth;         // 1 = single move search, 2 = + follow-up move
    int width;         // moves that get a follow-up search
    int budget;        // simulated ticks per plan (deterministic)
    int waitCalm;      // ticks to wait before looking again, calm board
    int waitActive;    // the same while blocks are moving
    double row;        // value of one row of garbage sent
    double chainGrowth;// extra value per row for longer chains
    double potential;  // weight of the chain potential of the result
    double comboCell;  // value of one garbage cell sent by a combo
    double clearCell;  // value of one garbage cell cleared on our board
    double effort;     // cost per input, so no move is made for nothing
    double flat;       // cost per unit of height difference between columns
    double tower;      // extra cost for a lone tall column
    double low;        // cost per row below the comfortable height
    double comfort;    // comfortable average stack height
    double danger;     // cost of the stack near the top (quadratic)
    double garbage;    // cost per garbage cell on the board
    double options;    // value per available match (flexibility)
    double follow;     // weight of the follow-up move's improvement
    double undo;       // cost of swapping a pair straight back
    double scramble;   // cost of standing still when about to top out
  };

 private:
  enum MoveKind {
    MOVE_SWAP, MOVE_DRAG, MOVE_RAISE, MOVE_WAIT
  };

  struct Move {
    MoveKind kind;
    int x, y;        // block (drag) or left swap cell
    int dir, len;    // drag direction (+1 right, -1 left) and columns
    int color;       // dragged block colour, to check every step
    std::vector<AIStrategyAction> actions;
  };

  struct Line {
    Move move;
    ProSim end;
    double value;    // what this move alone is worth
    double total;    // with the follow-up search
    bool valid;
  };

  Params _p;
  int _simulated;

  // The last single swap, so the AI does not flip a pair back and forth.
  int _lastSwapX, _lastSwapY;
  unsigned _lastSwapTick;

  // The model's prediction of the board at the next plan() call.
  bool _havePrediction;
  ProSim _prediction;
  long _verifyChecks;
  long _verifyMisses;
  // Opponent garbage that arrived since the prediction was made is not a
  // model error; neither are colours the model had to invent.
  bool predictionUsable(const ProSim &now) const;

  void buildMoves(const ProSim &from, std::vector<Move> &moves, int maxDrag,
                  int pace, bool withRaise) const;
  void appendPath(int fromX, int fromY, int toX, int toY, int pace,
                  std::vector<AIStrategyAction> &out) const;
  static void appendAction(AIStrategyAction a, int pace, std::vector<AIStrategyAction> &out);
  // Buttons pressed (everything but waiting), for the effort cost.
  static int inputs(const Move &move);
  bool simulate(const ProSim &from, const Move &move, ProSim &out);
  void rollout(ProSim &m);
  double evaluate(const ProSim &m) const;
  double chainReward(int rows) const;
  double potential(const ProSim &m, int &triggers) const;
  void describe(const Line &line, const ProSim &base, unsigned ms) const;
};

#endif /* AI_PROSTRATEGY_H_ */
