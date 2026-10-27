
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
#include"../Rendering/GameRenderer.h"

#include <memory>

class GameState : public State {
 public:

  void goBack();
  virtual SDL_Texture *render();
  virtual void tick();

 protected:
  std::unique_ptr<Game> _game;
  std::unique_ptr<GameRenderer> _gameRenderer;
  std::unique_ptr<BoardController> _playerBoardController;
  std::vector<std::unique_ptr<BoardController>> _opponentBoardcontollers;
};
#endif /* GAMESTATE_H */

