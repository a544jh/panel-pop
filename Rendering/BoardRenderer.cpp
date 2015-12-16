/*
 * BoardRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "BoardRenderer.h"
#include "../Game/Board.h"
#include <iostream>
#include "ChainPopup.h"
#include "ComboPopup.h"
#include "../Game/GarbageBlock.h"

const int BoardRenderer::BOARD_WIDTH = 192;
const int BoardRenderer::BOARD_HEIGHT = 384;
const int BoardRenderer::TILE_SIZE = 32;

BoardRenderer::BoardRenderer(Board& board) :
		_board(board) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, BOARD_WIDTH, BOARD_HEIGHT);
}

void BoardRenderer::tick() {
	handleChain();
	handleCombo();
	handlePopups();
}

SDL_Texture* BoardRenderer::renderBoard() {
	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	drawBlocks();
	drawBufferRow();
	drawGarbageBlocks();
	//drawGrid();
	drawCursor();
	drawPopups();

	return _texture;
}

void BoardRenderer::drawGrid() {
	SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0x00, 0x00, 0xFF);

	for (int i = 0; i < 6; i++) {
		SDL_RenderDrawLine(_SDLRenderer, i * TILE_SIZE, 0, i * TILE_SIZE,
				TILE_SIZE * 12);
	}

	for (int i = 0; i < 12; i++) {
		SDL_RenderDrawLine(_SDLRenderer, 0, i * TILE_SIZE, TILE_SIZE * 6,
				i * TILE_SIZE);
	}
}

void BoardRenderer::drawBlocks() {
	SDL_SetRenderDrawBlendMode(_SDLRenderer, SDL_BLENDMODE_BLEND);
	for (int i = 0; i < Board::BOARD_HEIGHT; i++) {
		for (int j = 0; j < Board::BOARD_WIDTH; j++) {
			if (_board.getTile(i, j).type == BLOCK) {
				const Block& block = _board.getTile(i, j).b;

				SDL_Rect pos;
				pos.h = TILE_SIZE;
				pos.w = TILE_SIZE;
				pos.x = j * TILE_SIZE;
				pos.y = (BOARD_HEIGHT - (i + 1) * TILE_SIZE)
						- _board.getStackOffset();
				int xOffset = TILE_SIZE * (double) block._swapTimer
						/ _board.SWAP_DELAY;
				if (block._state == SWAPPING_RIGHT) {
					pos.x += xOffset;
				} else if (block._state == SWAPPING_LEFT) {
					pos.x -= xOffset;
				}
				//SDL_RenderFillRect(_SDLRenderer, &pos);
				if (block._explosionTimer > 45
						&& block._explosionTimer >= block._explosionAnimTicks) {
					continue;
				}

				SDL_Rect sheet = getBlockSprite(block);

				if (_board.getGraceTimer() > 0) {
					int px = _board.getGraceTimer() * TILE_SIZE
							/ ((32 * _board.getStackRaiseTicks()) / 2);
					SDL_Rect top = { sheet.x, sheet.y, sheet.w, TILE_SIZE - px };
					SDL_Rect bottom = { sheet.x, sheet.y + (TILE_SIZE - px),
							sheet.w, px };
					pos.h = top.h;
					SDL_RenderCopy(_SDLRenderer, _spriteSheet, &top, &pos);
					SDL_SetTextureColorMod(_spriteSheet, 0xFF, 0x80, 0x80);
					pos.h = bottom.h;
					pos.y += top.h;
					SDL_RenderCopy(_SDLRenderer, _spriteSheet, &bottom, &pos);
					SDL_SetTextureColorMod(_spriteSheet, 0xFF, 0xFF, 0xFF);
				} else {
					SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &pos);
				}
			}
		}
	}
}

SDL_Rect BoardRenderer::getBlockSprite(const Block& block) {
	SDL_Rect sheet;
	sheet.w = TILE_SIZE;
	sheet.h = TILE_SIZE;
	switch (block._color) {
	case BlockColor::CYAN:
		sheet.x = 64;
		break;
	case BlockColor::GREEN:
		sheet.x = 32;
		break;
	case BlockColor::PURPLE:
		sheet.x = 96;
		break;
	case BlockColor::RED:
		sheet.x = 128;
		break;
	case BlockColor::YELLOW:
		sheet.x = 0;
		break;
	default:
		sheet.x = 0;
	}
	if (block._state == EXPLODING) {
		if (block._explosionTimer <= 45) {
			if (block._explosionTimer % 2 == 0) {
				sheet.y = 0;
			} else {
				sheet.y = 160;
			}
		} else {
			if (block._explosionTimer < block._explosionAnimTicks) {
				sheet.y = 128;
			}
		}
	} else if (_board.getState() == Board::GAME_OVER) {
		sheet.y = 128;
	} else {
		sheet.y = 0;
	}
	return sheet;

}

void BoardRenderer::drawBufferRow() {
	SDL_SetTextureColorMod(_spriteSheet, 0x50, 0x50, 0x50);
	for (int i = 0; i < Board::BOARD_WIDTH; i++) {
		const Block block = _board.getBufferRow(i).b;
		SDL_Rect sheet = getBlockSprite(block);

		SDL_Rect pos;
		pos.h = TILE_SIZE;
		pos.w = TILE_SIZE;
		pos.x = i * TILE_SIZE;
		pos.y = (BOARD_HEIGHT) - _board.getStackOffset();
		SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &pos);

	}
	SDL_SetTextureColorMod(_spriteSheet, 0xff, 0xff, 0xff);
}

void BoardRenderer::drawGarbageBlocks() {
	SDL_SetRenderDrawColor(_SDLRenderer, 0x80, 0x00, 0x00, 0xFF);
	auto garbageBlocks = _board.getGarbageBlocks();
	for (auto it = garbageBlocks.begin(); it != garbageBlocks.end(); ++it) {
		SDL_Rect pos;
		pos.h = TILE_SIZE * it->getH();
		pos.w = TILE_SIZE * it->getW();
		pos.x = it->getX() * TILE_SIZE;
		pos.y = (BOARD_HEIGHT - (it->getY() + 1) * TILE_SIZE) - _board.getStackOffset();
		SDL_RenderFillRect(_SDLRenderer, &pos);
	}
}

void BoardRenderer::drawCursor() {
	SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0x00, 0x00, 0xFF);

	SDL_Rect cur;
	cur.y = (BOARD_HEIGHT - (_board.getCursorY() + 1) * TILE_SIZE)
			- _board.getStackOffset();
	cur.x = _board.getCursorX() * TILE_SIZE;
	cur.w = 2 * TILE_SIZE;
	cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur); //up
	cur.w = 2;
	cur.h = TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur); //left
	cur.x += 2 * TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur); //right
	cur.x = _board.getCursorX() * TILE_SIZE;
	cur.y += TILE_SIZE;
	cur.w = 2 * TILE_SIZE;
	cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur); //down

}

void BoardRenderer::handleChain() {
	if (_board.isTickChain()) {
		_popups.push_back(
				new ChainPopup(_board.getTickMatchCol() * TILE_SIZE + 5,
						(BOARD_HEIGHT
								- (_board.getTickMatchRow() + 1) * TILE_SIZE
								- _board.getStackOffset()),
						_board.getChainCounter(), 60));
	}
}

void BoardRenderer::handleCombo() {
	if (_board.getTickMatched() > 3) {
		_popups.push_back(
				new ComboPopup(_board.getTickMatchCol() * TILE_SIZE + 5,
						(BOARD_HEIGHT
								- (_board.getTickMatchRow() + 1) * TILE_SIZE
								- _board.getStackOffset()) + 30,
						_board.getTickMatched(), 60));
	}
}

BoardRenderer::~BoardRenderer() {
}

void BoardRenderer::drawPopups() {
	for (auto it = _popups.begin(); it != _popups.end(); ++it) {
		(*it)->renderToBoard();
	}
}

void BoardRenderer::handlePopups() {
	auto it = _popups.begin();
	while (it != _popups.end()) {
		if ((*it)->_alive) {
			(*it)->tick();
			++it;
		} else {
			delete *it;
			it = _popups.erase(it);
		}
	}
}
