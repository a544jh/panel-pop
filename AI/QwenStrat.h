/*
 * QwenStrat.h
 *
 * The default "VS AI" strategy: a one-ply search over all legal swaps, where
 * every candidate is played out on a copy of the board (SimBoard) for a few
 * seconds of game time and judged by how much it clears, how much garbage it
 * buries the opponent in, and how safe the resulting position is. The best
 * few candidates then get a second-ply lookahead, which is what makes the AI
 * build chains instead of only clearing what is directly matchable.
 *
 * See AGENTS.md for the tuning notes.
 */

#ifndef AI_QWENSTRAT_H_
#define AI_QWENSTRAT_H_

#include <vector>

#include "AIStrategy.h"
#include "SimBoard.h"

class QwenStrat : public AIStrategy {
 public:
  QwenStrat();

  const std::string& name() const override;
  const std::string& key() const override;

  std::vector<AIStrategyAction> plan(const Board &board) override;

  // Drops the plan timer: it counts the board's logic ticks, which start over
  // when a new round begins. Without this the AI would not search again until
  // the fresh board had ticked as far as the previous round had run.
  void reset() override;

  // Weight tuning: "height=12,holes=20" (see the .cpp).
  void setOptions(const std::string &options) override;


 private:
  struct Move {
    // x < 0 for the non-swap candidates: pass (-1), forced raise (-2).
    int x;
    int y;
    double value;
    double followUp;
    // Garbage cells this swap sets off on the spot (see digCellsForSwap()).
    int dig;
  };

  // Tuning knobs, see the comment on evaluate().
  struct Weights {
    double score;
    double garbage;
    double chainLevel;
    double pendingScore;
    double height;
    double heightSquare;
    double holes;
    double garbageRows;
    double panicBlocks;
    double topThreat;
    double matchOptions;
    double optionBlocks;
    double dig;
    double garbageNear;
    double raiseGarbage;
    double adjacency;
    double columnSpread;
    double raisePenalty;
    double death;
  };

  // Search shape: horizons in logic ticks, how many candidates get a second
  // ply, the wall clock budget, and the move value discounts.
  struct Tuning {
    int simTicks;
    int followupTicks;
    int lookaheadCandidates;
    unsigned budgetMs;
    double followupWeight;
    double travelCost;
    // Ticks to wait before searching again, calm / while blocks are moving.
    int replanTicks;
    int replanActiveTicks;
  };

  // Prints the effective configuration under PANELPOP_AI_DEBUG.
  void dump();

  mutable bool _dumped;
  Weights _w;
  Tuning _t;
  // Do not search again before the board had a chance to change.
  unsigned _nextPlanTick;

  // Value of a model position; base is only used for the deltas.
  double evaluate(const SimBoard &model, const SimBoard &base, bool survival) const;
  // Plays the model out for "ticks" and judges the result.
  double playAndValue(SimBoard &model, const SimBoard &base, int ticks, bool survival) const;
  // Swaps that would score on the spot (and how many blocks they clear, and
  // how much garbage those clears would set off).
  static void immediateMatches(const SimBoard &model, int &moves, int &blocks,
                               int &digCells);
  // Length of the same-colour run through (row, col), horizontally or vertically.
  static int runSpan(const SimBoard &model, int row, int col, bool horizontal);
  // Garbage cells adjacent to that run: what a match there would dig at.
  static int garbageAlongRun(const SimBoard &model, int row, int col, bool horizontal);
  // Garbage cells a swap at (x, y) sets off on the spot; 0 when it matches
  // nothing. This is the opportunity the search otherwise never sees: the
  // first move of a dig only *converts* garbage into blocks, the payoff comes
  // one cascade later.
  static int digCellsForSwap(const SimBoard &model, int x, int y);
  // Highest row holding garbage (-1 when there is none), and the number of
  // garbage rows within `rows` of the top row - a garbage carpet low down is a
  // material problem, one at row 10 is a deadline.
  static int highestGarbageRow(const SimBoard &model);
  static int garbageNearTop(const SimBoard &model, int rows);
  // Fills moves with every legal swap plus the forced raise and "pass",
  // ordered by value. The cursor distance is subtracted from the value, so
  // equal moves are picked by how quickly they can be played.
  int generateMoves(const SimBoard &base, std::vector<Move> &moves, bool survival,
                    int cursorX, int cursorY) const;

  static void appendCursorPath(int fromX, int fromY, int toX, int toY,
                               std::vector<AIStrategyAction> &out);
};

#endif /* AI_QWENSTRAT_H_ */
