/*
 * StateDump.h
 *
 * Read-only text/JSON snapshots of the running game, used by the debug
 * console. See AGENTS.md.
 */

#ifndef DEBUG_STATEDUMPH_
#define DEBUG_STATEDUMPH_

#include <string>

class State;
class Board;
class Menu;

namespace StateDump {

// "EndlessGameState", "TitleScreen", ... ("" when the state is unknown).
std::string stateName(State *state);

// Human-readable summary of the current state (console "state" command).
std::string summary(State *state);

// One board as an ASCII grid (console "board" command).
std::string board(const Board &b, int playerIndex);

// Menu items with the current selection marked (console "menu" command).
std::string menu(const Menu &menu);

// Machine-readable snapshot, one line of JSON (console "json" command).
std::string json(State *state);

} // namespace StateDump

#endif /* DEBUG_STATEDUMPH_ */
