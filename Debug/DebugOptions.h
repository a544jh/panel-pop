/*
 * DebugOptions.h
 *
 * Runtime switches for the headless debug mode (see AGENTS.md). Parsed once in
 * main() from the command line and from PANELPOP_* environment variables.
 */

#ifndef DEBUG_DEBUGOPTIONSH_
#define DEBUG_DEBUGOPTIONSH_

#include <string>

class DebugOptions {
 public:
  static DebugOptions &get();

  // Consumes the debug flags from argv, leaving the remaining args in place.
  void parse(int argc, char *argv[]);

  // dummy video/audio drivers: the game runs without a display.
  bool headless() const;
  // read commands from stdin and advance the game one logic tick at a time.
  bool console() const;
  // call State::render() for every stepped tick (off with --no-render).
  bool render() const;
  // stop after this many logic ticks; 0 = no limit.
  int maxTicks() const;
  // --help was given: main() prints usage and exits.
  bool usageRequested() const;

  static std::string usage();

 private:
  DebugOptions();

  bool _headless;
  bool _console;
  bool _render;
  int _maxTicks;
  bool _usage;

  static bool envFlag(const char *name, bool fallback);
  static int envInt(const char *name, int fallback);
};

#endif /* DEBUG_DEBUGOPTIONSH_ */
