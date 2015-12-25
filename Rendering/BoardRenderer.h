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
#include <SDL2/SDL.h>
#include <list>
#include "Popup.h"

class BoardRenderer : public Renderer {
public:
	static const int BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE;

	BoardRenderer(Board&);
	virtual ~BoardRenderer();
	void tick();
	SDL_Texture* renderBoard();
private:
	Board& _board;
	void drawGrid();
	void drawBlocks();
	void drawGarbageBlocks();
	void drawCursor();
	void drawBufferRow();
	void handleChain();
	void handleCombo();
	void handlePopups();
	void drawPopups();
	SDL_Rect getBlockSprite(const Block&);
	SDL_Rect getGarbageBlockSprite(int,int, const GarbageBlock&);
	std::list<Popup*> _popups;
};

#endif /* BOARDRENDERER_H_ */
