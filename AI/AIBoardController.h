#ifndef AIBOARDCONTROLLER_H
#define AIBOARDCONTROLLER_H

#include <memory>
#include <string>
#include <vector>

#include "../Game/BoardController.h"
#include "AIStrategy.h"

// Drives a board with an AIStrategy: asks the strategy for a list of actions
// and plays them back on the board, one action per tick.
class AIBoardController : public BoardController {
 public:
  // Uses the default strategy (AIStrategies::defaultKey()).
  AIBoardController(Board &);
  AIBoardController(Board &, std::unique_ptr<AIStrategy> strategy);

  void tick() override;

  const std::string &strategyName() const;
  const std::string &strategyKey() const;
  void setStrategy(std::unique_ptr<AIStrategy> strategy);

  virtual ~AIBoardController();

 private:
  // How often the strategy is asked for a new plan when the previous one is
  // used up (in logic ticks).
  static const int DECISION_TICKS = 1;

  std::unique_ptr<AIStrategy> _strategy;
  std::vector<AIStrategyAction> _actions;
  size_t _nextAction;
  // Board tick of the previous tick(), to notice the board restarting.
  unsigned int _lastTickRun;

  void doAction(AIStrategyAction action);
};

#endif /* AIBOARDCONTROLLER_H */
