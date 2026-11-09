/*
 * DebugConsole.h
 *
 * Interactive stdin console for the headless debug mode: inspects game state
 * and injects input, advancing the game one logic tick at a time.
 * See AGENTS.md for the command reference.
 */

#ifndef DEBUG_DEBUGCONSOLEH_
#define DEBUG_DEBUGCONSOLEH_

class DebugConsole {
 public:
  // Replaces StateManager::run() in --debug-stdin mode. Returns the exit code.
  static int run();
};

#endif /* DEBUG_DEBUGCONSOLEH_ */
