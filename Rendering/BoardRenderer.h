/*
 * BoardRenderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BOARDRENDERER_H_
#define BOARDRENDERER_H_

#include <list>

#include "../Game/Board.h"
#include "Popup.h"

class BoardRenderer : public Renderer {
 public:
  static const int BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE, CURSOR_ANIM_TICKS;
  static const int BLINK_TIME = 45;

  BoardRenderer(const Board &);
  virtual ~BoardRenderer();
  void tick();
  SDL_Texture *renderBoard();
 private:
  const Board &_board;
  void drawGrid();
  void drawBlocks();
  void drawGarbageBlocks();
  void drawCursor();
  void drawBufferRow();
  SDL_Rect getBlockSprite(const Block &);
  SDL_Rect getGarbageBlockSprite(int, int, const GarbageBlock &);
  void drawCountdown();
  void drawGameOver();

  SDL_Texture *_readyText;
  SDL_Texture *_3Text;
  SDL_Texture *_2Text;
  SDL_Texture *_1Text;
  SDL_Texture *_goText;
  SDL_Texture *_loseText;
  SDL_Texture *_winText;
};

#endif /* BOARDRENDERER_H_ */
