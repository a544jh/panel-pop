/*
 * GbQueueRenderer.h
 *
 *  Created on: 3.1.2016
 *      Author: axelw
 */

#ifndef RENDERING_GBQUEUERENDERER_H_
#define RENDERING_GBQUEUERENDERER_H_

#include "../Game/Board.h"
#include "Renderer.h"

class GbQueueRenderer : public Renderer {
 public:
  GbQueueRenderer(Board &);
  virtual ~GbQueueRenderer();
  SDL_Texture *renderQueue();
 private:
  Board &_board;
};

#endif /* RENDERING_GBQUEUERENDERER_H_ */
