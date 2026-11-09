/*
 * DebugInput.h
 *
 * Virtual keyboard for the debug console: injects key events without a real
 * keyboard. See AGENTS.md.
 */

#ifndef DEBUG_DEBUGINPUTH_
#define DEBUG_DEBUGINPUTH_

#include <cstdint>
#include <string>
#include <vector>

class DebugInput {
 public:
  // Injection is only active in --debug-stdin mode, so a normal game run
  // keeps reading SDL's keyboard state exactly as before.
  static bool enabled();

  // One-tick key press (applied on the next InputManager::poll(), released
  // again on the tick after that). Also pushes a real SDL_KEYDOWN/KEYUP pair
  // so the key-capture menu (getInputDownEvent) sees it too.
  static void press(int scancode);

  // Hold a key down / release it; held keys stay down across ticks.
  static void hold(int scancode);
  static void release(int scancode);

  // Whether a scancode currently reads as pressed because of an injection.
  // Consulted by KeyboardKey::isActive(), which reads SDL's key state directly.
  static bool active(int scancode);

  // Called by InputManager::poll() after the event pump: copies SDL's keyboard
  // state and applies the pending injections on top of it.
  static void applyTo(const uint8_t *sdlKeys, int len, std::vector<uint8_t> &out);

  // "right", "x", "escape", "KUp", "SDL_SCANCODE_RIGHT" -> scancode, or -1.
  static int scancodeFromName(const std::string &name);
  // scancode -> short canonical name for printing, e.g. "right".
  static std::string nameOf(int scancode);

 private:
  DebugInput() = delete;
};

#endif /* DEBUG_DEBUGINPUTH_ */
