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


class BoardRenderer: public Renderer {
public:
	static const int BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE, CURSOR_ANIM_MS;

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
	SDL_Rect getGarbageBlockSprite(int, int, const GarbageBlock&);
	std::list<Popup*> _popups;

	SDL_Texture* _readyText;
	SDL_Texture* _3Text;
	SDL_Texture* _2Text;
	SDL_Texture* _1Text;
	SDL_Texture* _goText;
};

#endif /* BOARDRENDERER_H_ */
