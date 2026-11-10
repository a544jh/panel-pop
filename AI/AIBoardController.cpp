#include <utility>

#include "AIBoardController.h"

AIBoardController::AIBoardController(Board &board) :
    BoardController(board),
    _strategy(AIStrategies::create(AIStrategies::defaultKey())),
    _nextAction(0) {
}

AIBoardController::AIBoardController(Board &board, std::unique_ptr<AIStrategy> strategy) :
    BoardController(board),
    _strategy(std::move(strategy)),
    _nextAction(0) {
}

void AIBoardController::tick() {
  if (_board.getState() != Board::RUNNING || _strategy == nullptr) {
    return;
  }

  // Finish the current plan first; cursor moves and swaps are played one per
  // tick so the AI can travel and react quickly.
  if (_nextAction < _actions.size()) {
    doAction(_actions[_nextAction++]);
    return;
  }

  // The strategy rate limits itself (it knows when the board is settling);
  // here we only make sure it gets asked as soon as it ran out of actions.
  if (_board.getTicksRun() % DECISION_TICKS != 0) {
    return;
  }

  _actions = _strategy->plan(_board);
  _nextAction = 0;
}

void AIBoardController::doAction(AIStrategyAction action) {
  switch (action) {
    case AI_UP:_board.inputMoveCursor(Direction::UP);
      break;
    case AI_RIGHT:_board.inputMoveCursor(Direction::RIGHT);
      break;
    case AI_DOWN:_board.inputMoveCursor(Direction::DOWN);
      break;
    case AI_LEFT:_board.inputMoveCursor(Direction::LEFT);
      break;
    case AI_SWAP:_board.inputSwapBlocks();
      break;
    case AI_RAISE:_board.inputForceStackRaise();
      break;
    case AI_WAIT:
      break;
  }
}

const std::string &AIBoardController::strategyName() const {
  static const std::string empty;
  return _strategy ? _strategy->name() : empty;
}

const std::string &AIBoardController::strategyKey() const {
  static const std::string empty;
  return _strategy ? _strategy->key() : empty;
}

void AIBoardController::setStrategy(std::unique_ptr<AIStrategy> strategy) {
  _strategy = std::move(strategy);
  _actions.clear();
  _nextAction = 0;
}

AIBoardController::~AIBoardController() {
}
