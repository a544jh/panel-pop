/*
 * DebugConsole.cpp
 */

#include "DebugConsole.h"

#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "../Game/Board.h"
#include "../Game/Game.h"
#include "../SDLContext.h"
#include "../States/AIGameState.h"
#include "../AI/AIStrategy.h"
#include "../States/DemoGameState.h"
#include "../States/EndlessGameState.h"
#include "../States/OptionsMenuState.h"
#include "../States/State.h"
#include "../States/StateManager.h"
#include "../States/TitleScreen.h"
#include "../States/VsGameState.h"
#include "DebugInput.h"
#include "DebugOptions.h"
#include "StateDump.h"

namespace {

volatile sig_atomic_t s_interrupted = 0;

void onSigint(int) {
    s_interrupted = 1;
}

std::vector<std::string> split(const std::string &line) {
    std::vector<std::string> parts;
    std::istringstream is(line);
    std::string part;
    while (is >> part) {
        parts.push_back(part);
    }
    return parts;
}

const Board *boardOf(State *state, int index) {
    GameState *gameState = dynamic_cast<GameState *>(state);
    if (gameState == nullptr || gameState->getGame() == nullptr) {
        return nullptr;
    }
    return &gameState->getGame()->getBoard(index);
}

// Grabs whatever was last presented to the window. Only useful together with
// rendering (see --no-render).
bool screenshot(const std::string &path) {
    SDL_Renderer *renderer = SDLContext::getInstance().getRenderer();
    if (renderer == nullptr) {
        std::cout << "no renderer\n";
        return false;
    }
    int w = 0, h = 0;
    if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0) {
        std::cout << SDL_GetError() << "\n";
        return false;
    }
    SDL_Surface *surface =
        SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR8888);
    if (surface == nullptr) {
        std::cout << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetRenderTarget(renderer, nullptr);
    int rc = SDL_RenderReadPixels(renderer, nullptr, surface->format->format,
                                  surface->pixels, surface->pitch);
    bool ok = (rc == 0);
    if (ok) {
        ok = (SDL_SaveBMP(surface, path.c_str()) == 0);
    }
    if (!ok) {
        std::cout << "screenshot failed: " << SDL_GetError() << "\n";
    } else {
        std::cout << "wrote " << path << " (" << w << "x" << h << ")\n";
    }
    SDL_FreeSurface(surface);
    return ok;
}

} // namespace

int DebugConsole::run() {
    StateManager &sm = StateManager::getInstance();
    const DebugOptions &opts = DebugOptions::get();

    std::signal(SIGINT, onSigint);

    const bool tty = (isatty(0) != 0);
    bool echo = !tty;
    int budget = opts.maxTicks();

    std::cout << "# panel-pop debug console - type 'help' for commands\n" << std::flush;

    std::string line;
    while (sm.running() && !s_interrupted) {
        if (!tty) {
            std::cout << "pp> " << std::flush;
        }
        if (!std::getline(std::cin, line)) {
            std::cout << "# stdin closed\n" << std::flush;
            break;
        }
        if (echo) {
            std::cout << "# " << line << "\n" << std::flush;
        }

        std::vector<std::string> argv = split(line);
        if (argv.empty()) {
            continue;
        }
        const std::string &cmd = argv[0];

        if (cmd == "quit" || cmd == "q" || cmd == "exit") {
            sm.quit();
        } else if (cmd == "help" || cmd == "?") {
            std::cout <<
                "commands:\n"
                "  state | s            summary of the current state\n"
                "  board [1|2] | b      ASCII dump of a board\n"
                "  json                 machine-readable snapshot of everything\n"
                "  menu                 current menu items and selection\n"
                "  press <key>          queue a one-tick key press (applied on the next tick)\n"
                "  down <key>           hold a key down\n"
                "  up <key>             release a held key\n"
                "  send <key> [n]       press <key>, then advance n ticks (default 1)\n"
                "  tick [n] | t [n]     advance n logic ticks (default 1)\n"
                "  run [n]              alias for tick, prints a summary afterwards\n"
                "  wait <ms>            real-time sleep, for SDL_GetTicks-based timers\n"
                "  goto <state>         switch state: title|endless|vs|ai|demo|options\n"
                "  shot <file.bmp>      save the rendered frame as a BMP\n"
                "  strategy [name]      show or set the AI strategy (ai/demo games)\n"
                "  echo on|off          echo piped commands (default: on when stdin is not a tty)\n"
                "  quit | q             leave\n"
                "\n"
                "keys: up down left right x z esc return space tab ctrl alt shift pause,\n"
                "      single characters, SDL scancode names (RIGHT), and ini bindings (KUp)\n";
        } else if (cmd == "state" || cmd == "s") {
            std::cout << StateDump::summary(sm.getCurrentState()) << std::flush;
        } else if (cmd == "json") {
            std::cout << StateDump::json(sm.getCurrentState()) << "\n" << std::flush;
        } else if (cmd == "board" || cmd == "b") {
            int index = (argv.size() > 1) ? atoi(argv[1].c_str()) - 1 : 0;
            const Board *board = boardOf(sm.getCurrentState(), index);
            if (board == nullptr) {
                std::cout << "no board for state " << StateDump::stateName(sm.getCurrentState()) << "\n";
            } else {
                std::cout << StateDump::board(*board, index + 1) << std::flush;
            }
        } else if (cmd == "menu") {
            TitleScreen *title = dynamic_cast<TitleScreen *>(sm.getCurrentState());
            GameState *gameState = dynamic_cast<GameState *>(sm.getCurrentState());
            OptionsMenuState *options = dynamic_cast<OptionsMenuState *>(sm.getCurrentState());
            if (title != nullptr) {
                std::cout << StateDump::menu(title->getMenu()) << std::flush;
            } else if (options != nullptr && options->getCurrentMenu() != nullptr) {
                std::cout << StateDump::menu(*options->getCurrentMenu()) << std::flush;
            } else if (gameState != nullptr && gameState->getGame()->isPaused()) {
                std::cout << StateDump::menu(gameState->getGame()->getPauseMenu()) << std::flush;
            } else {
                std::cout << "no menu in " << StateDump::stateName(sm.getCurrentState()) << "\n";
            }
        } else if (cmd == "press") {
            int sc = DebugInput::scancodeFromName(argv.size() > 1 ? argv[1] : "");
            if (sc < 0) {
                std::cout << "unknown key\n";
            } else {
                DebugInput::press(sc);
                std::cout << "queued " << DebugInput::nameOf(sc) << " (advance with 'tick')\n";
            }
        } else if (cmd == "down" || cmd == "up") {
            int sc = DebugInput::scancodeFromName(argv.size() > 1 ? argv[1] : "");
            if (sc < 0) {
                std::cout << "unknown key\n";
            } else if (cmd == "down") {
                DebugInput::hold(sc);
            } else {
                DebugInput::release(sc);
            }
        } else if (cmd == "send") {
            int sc = DebugInput::scancodeFromName(argv.size() > 1 ? argv[1] : "");
            if (sc < 0) {
                std::cout << "unknown key\n";
            } else {
                DebugInput::press(sc);
                int n = (argv.size() > 2) ? atoi(argv[2].c_str()) : 1;
                for (int i = 0; i < n && sm.running() && !s_interrupted; ++i) {
                    sm.step(opts.render());
                }
            }
        } else if (cmd == "tick" || cmd == "t" || cmd == "run") {
            int n = (argv.size() > 1) ? atoi(argv[1].c_str()) : 1;
            if (n < 1) {
                n = 1;
            }
            for (int i = 0; i < n && sm.running() && !s_interrupted; ++i) {
                sm.step(opts.render());
            }
            if (cmd == "run") {
                std::cout << StateDump::summary(sm.getCurrentState()) << std::flush;
            }
        } else if (cmd == "wait") {
            int ms = (argv.size() > 1) ? atoi(argv[1].c_str()) : 1000;
            SDL_Delay((Uint32) ms);
        } else if (cmd == "goto") {
            const std::string target = argv.size() > 1 ? argv[1] : "";
            if (target == "title") {
                sm.switchToState(new TitleScreen());
            } else if (target == "endless") {
                sm.switchToState(new EndlessGameState());
            } else if (target == "vs") {
                sm.switchToState(new VsGameState());
            } else if (target == "ai") {
                sm.switchToState(new AIGameState());
            } else if (target == "demo") {
                sm.switchToState(new DemoGameState());
            } else if (target == "options") {
                sm.switchToState(new OptionsMenuState());
            } else {
                std::cout << "usage: goto title|endless|vs|ai|demo|options\n";
                continue;
            }
            sm.step(false); // apply the switch
            std::cout << "state: " << StateDump::stateName(sm.getCurrentState()) << "\n";
        } else if (cmd == "strategy" || cmd == "ai") {
            GameState *gameState = dynamic_cast<GameState *>(sm.getCurrentState());
            if (gameState == nullptr) {
                std::cout << "no game in " << StateDump::stateName(sm.getCurrentState()) << "\n";
            } else if (argv.size() > 1) {
                if (argv.size() > 2) {
                    gameState->setAiStrategy(argv[1], atoi(argv[2].c_str()) - 1);
                } else {
                    gameState->setAiStrategy(argv[1]);
                }
                const std::string name = gameState->aiStrategyName();
                std::cout << (name.empty() ? std::string("no AI board") : "ai strategy: " + name) << "\n";
            } else {
                const std::string name = gameState->aiStrategyName();
                std::cout << (name.empty() ? std::string("no AI board") : "ai strategy: " + name) << "\n";
                std::cout << "available:";
                for (const std::string &key : AIStrategies::keys()) {
                    std::cout << ' ' << key;
                }
                std::cout << "\n";
            }
        } else if (cmd == "shot") {
            screenshot(argv.size() > 1 ? argv[1] : "screenshot.bmp");
        } else if (cmd == "echo") {
            echo = (argv.size() > 1 && argv[1] == "on");
        } else {
            std::cout << "unknown command '" << cmd << "' (try 'help')\n";
        }

        if (budget > 0 && sm.logicTicks() >= budget) {
            std::cout << "# reached --frames=" << budget << " ticks\n" << std::flush;
            break;
        }
        std::cout << std::flush;
    }

    std::signal(SIGINT, SIG_DFL);
    std::cout << "# stopped after " << sm.logicTicks() << " logic ticks\n" << std::flush;
    return 0;
}
