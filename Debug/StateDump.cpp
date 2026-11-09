/*
 * StateDump.cpp
 */

#include "StateDump.h"

#include <cctype>
#include <sstream>
#include <string>

#include "../Game/Block.h"
#include "../Game/Board.h"
#include "../Game/EndlessGame.h"
#include "../Game/Game.h"
#include "../Game/GarbageBlock.h"
#include "../Game/VsGame.h"
#include "../Menus/Menu.h"
#include "../Menus/MenuItem.h"
#include "../States/GameState.h"
#include "../States/OptionsMenuState.h"
#include "../States/State.h"
#include "../States/TitleScreen.h"
#include "../States/AIGameState.h"
#include "../States/DemoGameState.h"
#include "../States/EndlessGameState.h"
#include "../States/FadeTransition.h"
#include "../States/VsGameState.h"

namespace {

char colorChar(BlockColor color) {
    switch (color) {
        case PURPLE:return 'P';
        case YELLOW:return 'Y';
        case RED:return 'R';
        case CYAN:return 'C';
        case GREEN:return 'G';
        default:return '?';
    }
}

char tileChar(const Board::Tile &tile) {
    if (tile.type == AIR) {
        return '.';
    }
    if (tile.type == GARBAGE) {
        bool gray = tile.g != nullptr && tile.g->getType() == GarbageBlockType::GRAY;
        return gray ? 'g' : '#';
    }
    if (tile.b._state == MATCHED || tile.b._state == EXPLODING) {
        return '*';
    }
    char c = colorChar(tile.b._color);
    if (tile.chain) {
        c = (char) std::tolower((unsigned char) c);
    }
    return c;
}

const char *boardStateName(Board::BoardState state) {
    switch (state) {
        case Board::RUNNING:return "RUNNING";
        case Board::COUNTDOWN:return "COUNTDOWN";
        case Board::WON:return "WON";
        case Board::GAME_OVER:return "GAME_OVER";
    }
    return "?";
}

const char *gameStateName(Game::State state) {
    switch (state) {
        case Game::State::RUNNING:return "RUNNING";
        case Game::State::PAUSED:return "PAUSED";
        case Game::State::ENDED:return "ENDED";
    }
    return "?";
}

const char *gameTypeName(const Game *game) {
    if (dynamic_cast<const EndlessGame *>(game) != nullptr) {
        return "EndlessGame";
    }
    if (dynamic_cast<const VsGame *>(game) != nullptr) {
        return "VsGame";
    }
    return "Game";
}

int boardCount(const Game *game) {
    return dynamic_cast<const VsGame *>(game) != nullptr ? 2 : 1;
}

std::string gridRow(const Board &b, int row) {
    std::string rowStr;
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        if (col > 0) {
            rowStr += ' ';
        }
        rowStr += tileChar(b.getTile(row, col));
    }
    return rowStr;
}

void appendMenu(std::ostream &os, const Menu &menu) {
    os << StateDump::menu(menu);
}

} // namespace

namespace StateDump {

std::string stateName(State *state) {
    if (dynamic_cast<TitleScreen *>(state) != nullptr) {
        return "TitleScreen";
    }
    if (dynamic_cast<OptionsMenuState *>(state) != nullptr) {
        return "OptionsMenuState";
    }
    // Most specific first: all of these are GameStates.
    if (dynamic_cast<EndlessGameState *>(state) != nullptr) {
        return "EndlessGameState";
    }
    if (dynamic_cast<VsGameState *>(state) != nullptr) {
        return "VsGameState";
    }
    if (dynamic_cast<AIGameState *>(state) != nullptr) {
        return "AIGameState";
    }
    if (dynamic_cast<DemoGameState *>(state) != nullptr) {
        return "DemoGameState";
    }
    if (dynamic_cast<GameState *>(state) != nullptr) {
        return "GameState";
    }
    if (dynamic_cast<FadeTransition *>(state) != nullptr) {
        return "FadeTransition";
    }
    return "";
}

std::string summary(State *state) {
    std::ostringstream os;
    os << stateName(state) << "\n";

    if (TitleScreen *title = dynamic_cast<TitleScreen *>(state)) {
        appendMenu(os, title->getMenu());
        return os.str();
    }

    if (OptionsMenuState *options = dynamic_cast<OptionsMenuState *>(state)) {
        if (Menu *menu = options->getCurrentMenu()) {
            appendMenu(os, *menu);
        }
        return os.str();
    }

    GameState *gameState = dynamic_cast<GameState *>(state);
    if (gameState == nullptr) {
        os << "  (no inspection data for this state)\n";
        return os.str();
    }

    Game *game = gameState->getGame();
    os << "  game:  " << gameTypeName(game) << " " << gameStateName(game->getState())
       << " ticks=" << game->getTicksRun() << " ms=" << game->getTime()
       << " panic=" << (game->isPanic() ? 1 : 0) << "\n";

    if (const EndlessGame *endless = dynamic_cast<const EndlessGame *>(game)) {
        os << "  high score: " << endless->getHighScore() << "\n";
    }
    if (const VsGame *vs = dynamic_cast<const VsGame *>(game)) {
        os << "  p1: points=" << vs->getP1Points() << " matchPoints=" << vs->getP1MatchPoints() << "\n"
           << "  p2: points=" << vs->getP2Points() << " matchPoints=" << vs->getP2MatchPoints() << "\n";
    }
    if (game->isPaused()) {
        appendMenu(os, game->getPauseMenu());
    }

    for (int i = 0; i < boardCount(game); ++i) {
        const Board &b = game->getBoard(i);
        os << "  board p" << (i + 1) << ": " << boardStateName(b.getState())
           << " score=" << b.getScore()
           << " cursor=(" << b.getCursorX() << "," << b.getCursorY() << ")"
           << " stackOffset=" << b.getStackOffset()
           << " chainCounter=" << b.getChainCounter()
           << " lastChain=" << b.getLastChain()
           << " grace=" << b.getGraceTimer()
           << " garbageBlocks=" << b.getGarbageBlocks().size()
           << " queued=" << b.getGarbageQueue().size()
           << " countdownState=" << b.getCountdownState()
           << "\n";
    }
    return os.str();
}

std::string board(const Board &b, int playerIndex) {
    std::ostringstream os;
    os << "board p" << playerIndex << " (row " << Board::TOP_ROW
       << " is the top visible row, row 0 the bottom)\n";

    os << "      ";
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        os << " " << col << " ";
    }
    os << "\n";

    for (int row = Board::TOP_ROW; row >= 0; --row) {
        os << "  " << (row < 10 ? " " : "") << row << " |" << gridRow(b, row) << "|";
        if (b.getCursorY() == row) {
            os << "   cursor x=" << b.getCursorX();
        }
        os << "\n";
    }

    os << "  buf |";
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        if (col > 0) {
            os << ' ';
        }
        os << tileChar(b.getBufferRow(col));
    }
    os << "|   stackOffset=" << b.getStackOffset() << "\n";

    os << "  warn:";
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        os << (b.getWarnColumn(col) ? " !" : " .");
    }
    os << "\n";

    os << "  chars: . empty  P Y R C G colors  p y r c g chain-linked  * matched/exploding"
          "  # garbage  g gray garbage\n";
    return os.str();
}

std::string menu(const Menu &menu) {
    std::ostringstream os;
    const std::vector<MenuItem> &items = menu.getItems();
    unsigned selection = menu.getSelection();
    for (unsigned i = 0; i < items.size(); ++i) {
        const MenuItem &item = items[i];
        os << "    " << (i == selection ? "> " : "  ") << i << " " << item.getName();
        if (!item.getOptionString().empty()) {
            os << " [" << item.getOptionString() << "]";
        }
        os << "\n";
    }
    if (items.empty()) {
        os << "    (empty menu)\n";
    }
    return os.str();
}

std::string json(State *state) {
    std::ostringstream os;
    os << "{\"state\":\"" << stateName(state) << "\"";

    if (TitleScreen *title = dynamic_cast<TitleScreen *>(state)) {
        const Menu &menu = title->getMenu();
        os << ",\"menu\":{\"selection\":" << menu.getSelection()
           << ",\"items\":[";
        const std::vector<MenuItem> &items = menu.getItems();
        for (size_t i = 0; i < items.size(); ++i) {
            os << (i ? "," : "") << "{\"name\":\"" << items[i].getName()
               << "\",\"selected\":" << (i == menu.getSelection() ? "true" : "false") << "}";
        }
        os << "]}";
        os << "}";
        return os.str();
    }

    GameState *gameState = dynamic_cast<GameState *>(state);
    if (gameState == nullptr) {
        os << "}";
        return os.str();
    }

    Game *game = gameState->getGame();
    os << ",\"game\":{\"type\":\"" << gameTypeName(game)
       << "\",\"state\":\"" << gameStateName(game->getState())
       << "\",\"ticks\":" << game->getTicksRun()
       << ",\"ms\":" << game->getTime()
       << ",\"panic\":" << (game->isPanic() ? "true" : "false");
    if (const EndlessGame *endless = dynamic_cast<const EndlessGame *>(game)) {
        os << ",\"highScore\":" << endless->getHighScore();
    }
    if (const VsGame *vs = dynamic_cast<const VsGame *>(game)) {
        os << ",\"p1Points\":" << vs->getP1Points()
           << ",\"p2Points\":" << vs->getP2Points()
           << ",\"p1MatchPoints\":" << vs->getP1MatchPoints()
           << ",\"p2MatchPoints\":" << vs->getP2MatchPoints();
    }
    os << "},\"boards\":[";

    for (int i = 0; i < boardCount(game); ++i) {
        const Board &b = game->getBoard(i);
        if (i > 0) {
            os << ",";
        }
        os << "{\"player\":" << (i + 1)
           << ",\"boardState\":\"" << boardStateName(b.getState())
           << "\",\"score\":" << b.getScore()
           << ",\"cursorX\":" << b.getCursorX()
           << ",\"cursorY\":" << b.getCursorY()
           << ",\"stackOffset\":" << b.getStackOffset()
           << ",\"stackRaiseTicks\":" << b.getStackRaiseTicks()
           << ",\"stackRaiseTimer\":" << b.getStackRaiseTimer()
           << ",\"stackRaiseForced\":" << (b.isStackRaiseForced() ? "true" : "false")
           << ",\"hasActiveBlocks\":" << (b.hasActiveBlocks() ? "true" : "false")
           << ",\"chainCounter\":" << b.getChainCounter()
           << ",\"lastChain\":" << b.getLastChain()
           << ",\"graceTimer\":" << b.getGraceTimer()
           << ",\"ticksRun\":" << b.getTicksRun()
           << ",\"panic\":" << (b.isPanic() ? "true" : "false")
           << ",\"countdownState\":" << b.getCountdownState()
           << ",\"garbageBlocks\":" << b.getGarbageBlocks().size()
           << ",\"garbageQueued\":" << b.getGarbageQueue().size()
           << ",\"warnColumns\":[";
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            os << (col ? "," : "") << (b.getWarnColumn(col) ? 1 : 0);
        }
        os << "],\"grid\":[";
        for (int row = Board::TOP_ROW; row >= 0; --row) {
            os << (row == Board::TOP_ROW ? "" : ",") << "\"";
            for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
                os << tileChar(b.getTile(row, col));
            }
            os << "\"";
        }
        os << "],\"bufferRow\":\"";
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            os << tileChar(b.getBufferRow(col));
        }
        os << "\"}";
    }

    os << "]}";
    return os.str();
}

} // namespace StateDump
