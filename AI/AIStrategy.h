/*
 * AIStrategy.h
 *
 * Interface for the strategies that drive computer controlled boards
 * ("VS AI" and the title screen demo). AIBoardController turns the action
 * lists produced here into board input. See AGENTS.md.
 */

#ifndef AI_AISTRATEGY_H_
#define AI_AISTRATEGY_H_

#include <memory>
#include <string>
#include <vector>

class Board;

// One primitive action: cursor movement, a swap at the current cursor
// position, a forced stack raise, or a no-op.
enum AIStrategyAction {
  AI_UP,
  AI_RIGHT,
  AI_DOWN,
  AI_LEFT,
  AI_SWAP,
  AI_RAISE,
  AI_WAIT
};

class AIStrategy {
 public:
  virtual ~AIStrategy();

  // Name shown to the user (options menu).
  virtual const std::string &name() const = 0;

  // Key used in panelpop.ini ([ai] strategy=...). Not the config spec:
  // callers parse options from the string handed to create(), so key() can
  // return a static string instead of a copy.
  virtual const std::string &key() const = 0;

  // Optional tuning string, "key=value" pairs separated by commas, e.g.
  // "qwen,height=12". Only QwenStrat has knobs; the base ignores it.
  virtual void setOptions(const std::string &options);

  // Actions to play on the given board. Called when the controller has run
  // out of queued actions; the board is only read here, the actions are
  // applied to it later.
  virtual std::vector<AIStrategyAction> plan(const Board &board) = 0;

  // The board this strategy plays restarted (a new round in a VS match):
  // Board::reset() puts a brand new board in place, so its tick counter
  // starts from 0 again and the tiles are unrelated to the old ones. Drop
  // anything keyed on the previous board - plan timing in particular, or a
  // strategy would keep waiting for a tick number that already passed.
  virtual void reset();
};

// Registry of the available strategies. The first entry is the default used
// when "VS AI" is started and nothing else is configured.
namespace AIStrategies {

// Keys and display names, in menu order.
const std::vector<std::string> &keys();
const std::vector<std::string> &names();

// Index of the default strategy in keys()/names().
size_t defaultIndex();
std::string defaultKey();

// Creates the strategy for a key, optionally with tuning options appended:
// "qwen,height=12". Falls back to the default for unknown keys.
std::unique_ptr<AIStrategy> create(const std::string &spec);

} // namespace AIStrategies

#endif /* AI_AISTRATEGY_H_ */
