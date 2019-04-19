/*
 * GameRenderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef VSGAMERENDERER_H_
#define VSGAMERENDERER_H_

#include "BoardRenderer.h"
#include "GameRenderer.h"
#include "GbQueueRenderer.h"

class VsGame;

class VsGameRenderer : public GameRenderer {
 public:
  VsGameRenderer(VsGame &);
  virtual ~VsGameRenderer();
  void tick();
  SDL_Texture *renderGame();

  static const int BOARD0_X = 58;
  static const int BOARD0_Y = 43;
  static const int BOARD1_X = 390;
  static const int BOARD1_Y = 43;

  void shakeBoard(int id, int duration);

 private:
  VsGame &_game;

  BoardRenderer _boardRenderer0;
  BoardRenderer _boardRenderer1;
  GbQueueRenderer _gbqr0;
  GbQueueRenderer _gbqr1;
  SDL_Texture *_bg;
  SDL_Texture *_2pbg;

  int _b0Shake;
  int _b1Shake;

  void renderBoard(int id);
  void renderStatsText();
  void renderMatchPoints();
  void handleShake();
};

#endif /* VSGAMERENDERER_H_ */
