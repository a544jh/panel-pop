/*
 * DebugInput.cpp
 */

#include "DebugInput.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>

#include "DebugOptions.h"

namespace {

// Names accepted by the debug console, in addition to SDL's own scancode names
// and the "K<ScancodeName>" form used in panelpop.ini.
const std::map<std::string, int> &aliases() {
    static const std::map<std::string, int> table = {
        {"up", SDL_SCANCODE_UP},
        {"down", SDL_SCANCODE_DOWN},
        {"left", SDL_SCANCODE_LEFT},
        {"right", SDL_SCANCODE_RIGHT},
        {"x", SDL_SCANCODE_X},
        {"z", SDL_SCANCODE_Z},
        {"esc", SDL_SCANCODE_ESCAPE},
        {"escape", SDL_SCANCODE_ESCAPE},
        {"return", SDL_SCANCODE_RETURN},
        {"enter", SDL_SCANCODE_RETURN},
        {"space", SDL_SCANCODE_SPACE},
        {"tab", SDL_SCANCODE_TAB},
        {"backspace", SDL_SCANCODE_BACKSPACE},
        {"ctrl", SDL_SCANCODE_LCTRL},
        {"lctrl", SDL_SCANCODE_LCTRL},
        {"rctrl", SDL_SCANCODE_RCTRL},
        {"alt", SDL_SCANCODE_LALT},
        {"lalt", SDL_SCANCODE_LALT},
        {"ralt", SDL_SCANCODE_RALT},
        {"shift", SDL_SCANCODE_LSHIFT},
        {"lshift", SDL_SCANCODE_LSHIFT},
        {"pause", SDL_SCANCODE_PAUSE},
    };
    return table;
}

// Keys injected by the console.
std::set<int> s_pendingPress;  // queued since the last tick
std::set<int> s_activePress;   // live during the current tick
std::set<int> s_held;          // held across ticks
bool s_enabled = false;

std::string lower(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

void pushSdlKey(int scancode, bool down) {
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event));
    event.type = down ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = down ? SDL_PRESSED : SDL_RELEASED;
    event.key.repeat = 0;
    event.key.keysym.scancode = (SDL_Scancode) scancode;
    event.key.keysym.sym = SDL_GetKeyFromScancode((SDL_Scancode) scancode);
    SDL_PushEvent(&event);
}

} // namespace

bool DebugInput::enabled() {
    // Enabled by --debug-stdin, or implicitly as soon as anything injects a key.
    return s_enabled || DebugOptions::get().console();
}

int DebugInput::scancodeFromName(const std::string &name) {
    if (name.empty()) {
        return -1;
    }

    auto it = aliases().find(lower(name));
    if (it != aliases().end()) {
        return it->second;
    }

    // "KUp" - the binding format written to panelpop.ini.
    if (name[0] == 'K' && name.size() > 1) {
        SDL_Scancode sc = SDL_GetScancodeFromName(name.substr(1).c_str());
        if (sc != SDL_SCANCODE_UNKNOWN) {
            return sc;
        }
    }

    // SDL's own names, upper-cased: "RIGHT", "SDL_SCANCODE_RIGHT".
    std::string upper = name;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    if (upper.compare(0, 14, "SDL_SCANCODE_") == 0) {
        upper = upper.substr(14);
    }
    SDL_Scancode sc = SDL_GetScancodeFromName(upper.c_str());
    return sc == SDL_SCANCODE_UNKNOWN ? -1 : (int) sc;
}

std::string DebugInput::nameOf(int scancode) {
    std::string name = lower(SDL_GetScancodeName((SDL_Scancode) scancode));
    return name.empty() ? std::string("?") : name;
}

void DebugInput::press(int scancode) {
    s_enabled = true;
    s_held.erase(scancode);
    s_pendingPress.insert(scancode);
    // A real event pair as well, so the key-capture menu (which works off the
    // event queue instead of the key state) also sees the press.
    pushSdlKey(scancode, true);
    pushSdlKey(scancode, false);
}

void DebugInput::hold(int scancode) {
    s_enabled = true;
    s_pendingPress.erase(scancode);
    s_held.insert(scancode);
    pushSdlKey(scancode, true);
}

void DebugInput::release(int scancode) {
    s_enabled = true;
    s_held.erase(scancode);
    s_pendingPress.erase(scancode);
    pushSdlKey(scancode, false);
}

void DebugInput::applyTo(const uint8_t *sdlKeys, int len, std::vector<uint8_t> &out) {
    // A press lives exactly one tick: what was queued becomes live, what was
    // live last tick is dropped (a swap() would ping-pong it back forever).
    s_activePress = s_pendingPress;
    s_pendingPress.clear();

    out.assign(sdlKeys, sdlKeys + len);
    for (int sc : s_held) {
        if (sc >= 0 && sc < len) {
            out[sc] = 1;
        }
    }
    for (int sc : s_activePress) {
        if (sc >= 0 && sc < len) {
            out[sc] = 1;
        }
    }
}

bool DebugInput::active(int scancode) {
    if (!s_enabled) {
        return false;
    }
    return s_held.count(scancode) > 0 || s_activePress.count(scancode) > 0;
}
