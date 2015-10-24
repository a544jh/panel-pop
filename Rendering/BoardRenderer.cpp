/*
 * BoardRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "BoardRenderer.h"
#include "ChainPanel.h"
#include <iostream>

const int BoardRenderer::BOARD_WIDTH = 192;
const int BoardRenderer::BOARD_HEIGHT = 384;
const int BoardRenderer::TILE_SIZE = 32;

BoardRenderer::BoardRenderer(Board& board) :
		_board(board) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, BOARD_WIDTH, BOARD_HEIGHT);
}

SDL_Texture* BoardRenderer::renderBoard() {

	handleChain();
	handleScorePanels();

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	drawBlocks();
	drawBufferRow();
	//drawGrid();
	drawCursor();

	drawScorePanels();

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
			if (_board._tiles[i][j].type == BLOCK) {
				Block& block = _board._tiles[i][j].b;

				SDL_Rect pos;
				pos.h = TILE_SIZE;
				pos.w = TILE_SIZE;
				pos.x = j * TILE_SIZE;
				pos.y = (BOARD_HEIGHT - (i + 1) * TILE_SIZE)
						- _board._stackOffset;
				//SDL_RenderFillRect(_SDLRenderer, &pos);
				if (block._explosionTimer > 45
						&& block._explosionTimer >= block._explosionAnimTicks) {
					continue;
				}

				SDL_Rect sheet = getBlockSprite(block);

				SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &pos);
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

	} else {
		sheet.y = 0;
	}
	return sheet;

}

void BoardRenderer::drawBufferRow() {
	for (int i = 0; i < Board::BOARD_WIDTH; i++) {
		Block& block = _board._bufferRow[i].b;
		SDL_Rect sheet = getBlockSprite(block);
		SDL_SetTextureColorMod(_spriteSheet,0x50,0x50,0x50);
		SDL_Rect pos;
		pos.h = TILE_SIZE;
		pos.w = TILE_SIZE;
		pos.x = i * TILE_SIZE;
		pos.y = (BOARD_HEIGHT) - _board._stackOffset;
		SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &pos);
		SDL_SetTextureColorMod(_spriteSheet,0xff,0xff,0xff);
	}
}

void BoardRenderer::drawCursor() {
	SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0x00, 0x00, 0xFF);

	SDL_Rect cur;
	cur.y = (BOARD_HEIGHT - (_board._cursorY + 1) * TILE_SIZE)
			- _board._stackOffset;
	cur.x = _board._cursorX * TILE_SIZE;
	cur.w = 2 * TILE_SIZE;
	cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur); //up
	cur.w = 2;
	cur.h = TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur); //left
	cur.x += 2 * TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur); //right
	cur.x = _board._cursorX * TILE_SIZE;
	cur.y += TILE_SIZE;
	cur.w = 2 * TILE_SIZE;
	cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur); //down

}

void BoardRenderer::handleChain() {
	if (_board._tickChain) {
		//TODO:proper_effect
		_scorePanels.push_back(
				new ChainPanel(_board._tickChainCol * TILE_SIZE + 5,
						(BOARD_HEIGHT - (_board._tickChainRow + 1) * TILE_SIZE)
								- _board._stackOffset, 0));
		std::cout << _board._chainCounter << "x" << std::endl;
	}
}

BoardRenderer::~BoardRenderer() {
	SDL_DestroyTexture(_texture);
}

void BoardRenderer::drawScorePanels() {
	for (auto it = _scorePanels.begin(); it != _scorePanels.end(); ++it) {

		SDL_Rect dst;
		dst.x = (*it)->_x;
		dst.y = (*it)->_y;
		dst.h = (*it)->_h;
		dst.w = (*it)->_w;
		SDL_SetRenderTarget(_SDLRenderer, _texture);
		SDL_RenderCopy(_SDLRenderer, (*it)->_texture, NULL, &dst);
	}
}

void BoardRenderer::handleScorePanels() {
	auto it = _scorePanels.begin();
	while (it != _scorePanels.end()) {
		if ((*it)->_alive) {
			(*it)->tick();
			++it;
		} else {
			delete *it;
			it = _scorePanels.erase(it);
		}
	}
}
