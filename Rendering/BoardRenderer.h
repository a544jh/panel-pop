/*
 * BoardRenderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BOARDRENDERER_H_
#define BOARDRENDERER_H_

#include "../Game/Board.h"
#include "Renderer.h"
#include "ScorePanel.h"
#include <SDL2/SDL.h>
#include <list>

class BoardRenderer : public Renderer {
public:
	static const int BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE;

	BoardRenderer(Board&);
	virtual ~BoardRenderer();
	SDL_Texture* renderBoard();
private:
	Board& _board;
	void drawGrid();
	void drawBlocks();
	void drawCursor();
	void drawBufferRow();
	void handleChain();
	void handleScorePanels();
	void drawScorePanels();
	std::list<ScorePanel*> _scorePanels;
};

#endif /* BOARDRENDERER_H_ */
