/*
 * BasicStrategy.h
 *
 * The original AIBoardController behaviour, extracted into a strategy: look
 * for a vertical match and clear it, flatten the stack when things get high,
 * otherwise hit the D button. Kept selectable in the options menu.
 */

#ifndef AI_BASICSTRATEGY_H_
#define AI_BASICSTRATEGY_H_

#include "AIStrategy.h"

class BasicStrategy : public AIStrategy {
 public:
  BasicStrategy();

  const std::string& name() const override;
  const std::string& key() const override;

  std::vector<AIStrategyAction> plan(const Board &board) override;

 private:
  // Cursor moves to (x, y) from the current cursor position.
  static void cursorPath(int curX, int curY, int x, int y,
                         std::vector<AIStrategyAction> &out);
  // Swap the block at (x, y) all the way to (dx, y).
  static void blockMove(int x, int y, int dx, std::vector<AIStrategyAction> &out,
                        int &curX, int &curY);
};

#endif /* AI_BASICSTRATEGY_H_ */
