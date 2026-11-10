
/* 
 * File:   GameState.h
 * Author: axel
 *
 * Created on September 26, 2016, 10:16 PM
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "State.h"

#include"../Game/Board.h"
#include"../Game/Game.h"
#include"../Game/BoardController.h"

#include <string>
#include"../Rendering/GameRenderer.h"

class GameState : public State {
 public:

  void goBack();
  virtual SDL_Texture *render();
  virtual void tick();
  // The running game; used by the headless debug console (see AGENTS.md).
  Game *getGame() const;
  // Switches the strategy of all computer controlled boards; the name is a
  // key from AI/AIStrategy.h ("qwen", "basic", ...). No-op without one.
  void setAiStrategy(const std::string &key);
  // Same, for one board only: index is the Game::getBoard() number (0 is the
  // player board), or -1 for all of them. Lets the debug console pit two
  // strategies against each other.
  void setAiStrategy(const std::string &key, int index);
  // Name of the strategy in use ("" when no board is computer controlled).
  std::string aiStrategyName() const;

  virtual ~GameState();
 protected:
  Game *_game;
  GameRenderer *_gameRenderer;
  BoardController *_playerBoardController;
  std::vector<BoardController *> _opponentBoardcontollers;
};
#endif /* GAMESTATE_H */

