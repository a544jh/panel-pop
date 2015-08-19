/*
 * BoardRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "BoardRenderer.h"

const int BoardRenderer::BOARD_WIDTH = 192;
const int BoardRenderer::BOARD_HEIGHT = 384;
const int BoardRenderer::TILE_SIZE = 32;

BoardRenderer::BoardRenderer(Board& board) :
		_board(board) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, BOARD_WIDTH, BOARD_HEIGHT);
}

SDL_Texture* BoardRenderer::renderBoard() {
	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	drawBlocks();
	//drawGrid();
	drawCursor();

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
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 6; j++) {
			if (_board._tiles[i][j].type == BLOCK) {
				Block& block = _board._tiles[i][j].b;
				switch (block._color) {
				case BlockColor::CYAN:
					SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0xFF, 0xFF,
							0xFF);
					break;
				case BlockColor::GREEN:
					SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0xFF, 0x00,
							0xFF);
					break;
				case BlockColor::PURPLE:
					SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0x00, 0xFF,
							0xFF);
					break;
				case BlockColor::RED:
					SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0x00, 0x00,
							0xFF);
					break;
				case BlockColor::YELLOW:
					SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0x00,
							0xFF);
					break;
				default:
					SDL_SetRenderDrawColor(_SDLRenderer, 0xAA, 0xAA, 0xAA,
							0xFF);
				}
				if (block._state == EXPLODING){
					uint8_t r, g, b, a;
					SDL_GetRenderDrawColor(_SDLRenderer, &r, &g, &b, &a);
					SDL_SetRenderDrawColor(_SDLRenderer, r, g, b, 0x80);
				}
				SDL_Rect pos;
				pos.h = TILE_SIZE;
				pos.w = TILE_SIZE;
				pos.x = j * TILE_SIZE;
				pos.y = BOARD_HEIGHT - (i + 1) * TILE_SIZE;
				SDL_RenderFillRect(_SDLRenderer, &pos);
			}
		}
	}
}

void BoardRenderer::drawCursor(){
	SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0x00, 0x00, 0xFF);

	SDL_Rect cur;
	cur.y = BOARD_HEIGHT - (_board._cursorY + 1) * TILE_SIZE;
	cur.x = _board._cursorX * TILE_SIZE;
	cur.w = 2 * TILE_SIZE; cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur);//up
	cur.w = 2; cur.h = TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur);//left
	cur.x += 2 * TILE_SIZE;
	SDL_RenderFillRect(_SDLRenderer, &cur);//right
	cur.x = _board._cursorX * TILE_SIZE;
	cur.y += TILE_SIZE;
	cur.w = 2 * TILE_SIZE; cur.h = 2;
	SDL_RenderFillRect(_SDLRenderer, &cur);//down

}

BoardRenderer::~BoardRenderer() {
	SDL_DestroyTexture(_texture);
}

