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
const int BoardRenderer::CURSOR_ANIM_TICKS = 30;

BoardRenderer::BoardRenderer(Board& board) :
		_board(board) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, BOARD_WIDTH, BOARD_HEIGHT);
	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	_readyText = _SDLContext.makeTextureFromFont("READY", { 0, 0, 0 },
			_SDLContext._squareFont);
	_3Text = _SDLContext.makeTextureFromFont("3", { 0, 0, 0 },
			_SDLContext._squareFont);
	_2Text = _SDLContext.makeTextureFromFont("2", { 0, 0, 0 },
			_SDLContext._squareFont);
	_1Text = _SDLContext.makeTextureFromFont("1", { 0, 0, 0 },
			_SDLContext._squareFont);
	_goText = _SDLContext.makeTextureFromFont("GO!", { 0, 0, 0 },
			_SDLContext._squareFont);
}

void BoardRenderer::tick() {
	handleChain();
	handleCombo();
	handlePopups();
}

SDL_Texture* BoardRenderer::renderBoard() {
	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0x00);
	SDL_RenderClear(_SDLRenderer);

	drawBlocks();
	drawBufferRow();
	drawGarbageBlocks();
	//drawGrid();
	drawCursor();
	drawPopups();
	uint32_t time = SDL_GetTicks() - _board.getStartTime();
	if (_board.getState() == Board::COUNTDOWN) {
		SDL_Rect pos = { 2, 100 };
		SDL_QueryTexture(_readyText, NULL, NULL, &pos.w, &pos.h);
		pos.x = (BOARD_WIDTH - pos.w) / 2;
		SDL_RenderCopy(_SDLRenderer, _readyText, NULL, &pos);
		pos.y += 42;
		SDL_Texture* digit;

		if (time < Board::COUNTDOWN_MS / 3) {
			digit = _3Text;
		} else if (time < 2 * Board::COUNTDOWN_MS / 3) {
			digit = _2Text;
		} else {
			digit = _1Text;
		}
		SDL_QueryTexture(digit, NULL, NULL, &pos.w, &pos.h);
		pos.x = (BOARD_WIDTH - pos.w) / 2;
		SDL_RenderCopy(_SDLRenderer, digit, NULL, &pos);
	}
	if (time < Board::COUNTDOWN_MS + Board::COUNTDOWN_MS / 3
			&& time > Board::COUNTDOWN_MS) {
		SDL_Rect pos = { 0, 142 };
		SDL_QueryTexture(_goText, NULL, NULL, &pos.w, &pos.h);
		pos.x = (BOARD_WIDTH - pos.w) / 2;
		SDL_RenderCopy(_SDLRenderer, _goText, NULL, &pos);
	}

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
	if (_board.getState() == Board::COUNTDOWN) {
		SDL_SetTextureAlphaMod(_spriteSheet, 0x50);
	} else {
		SDL_SetTextureAlphaMod(_spriteSheet, 0xff);
	}
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
	SDL_Rect sprite;
	sprite.w = TILE_SIZE;
	sprite.h = TILE_SIZE;
	switch (block._color) {
	case BlockColor::CYAN:
		sprite.x = 64;
		break;
	case BlockColor::GREEN:
		sprite.x = 32;
		break;
	case BlockColor::PURPLE:
		sprite.x = 96;
		break;
	case BlockColor::RED:
		sprite.x = 128;
		break;
	case BlockColor::YELLOW:
		sprite.x = 0;
		break;
	default:
		sprite.x = 0;
	}
	if (block._state == EXPLODING) {
		if (block._explosionTimer <= 45) {
			if (block._explosionTimer % 2 == 0) {
				sprite.y = 0;
			} else {
				sprite.y = 160;
			}
		} else {
			if (block._explosionTimer < block._explosionAnimTicks) {
				sprite.y = 128;
			}
		}
	} else if (_board.getState() == Board::GAME_OVER) {
		sprite.y = 128;
	} else {
		sprite.y = 0;
	}
	return sprite;

}

SDL_Rect BoardRenderer::getGarbageBlockSprite(int rx, int ry,
		const GarbageBlock& b) {
	SDL_Rect sprite;
	sprite.w = TILE_SIZE;
	sprite.h = TILE_SIZE;
	sprite.y = 0;
	int h = b.getH();
	//handle transforming block as one smaller (after blinking)
	if (b.getState() == GarbageBlockState::TRANSFORMING
			&& b.getTransformationTimer() > Board::BASE_TRANSFORMATION_TICKS) {
		--h;
		--ry;
	}
	bool blink = b.getTransformationTimer() % 2 != 0
			&& b.getTransformationTimer() <= Board::BASE_TRANSFORMATION_TICKS;
	int t = 0;
	if (rx == 0) {
		t = 2; //right
	} else if (rx == b.getW() - 1) {
		t = 0; //left
	} else {
		t = 1; //middle
	}
	if (h == 1) {
		sprite.x = 224 + t * TILE_SIZE;
		if (blink) {
			sprite.y += TILE_SIZE;
		}
		return sprite;
	} else {
		int u = 0;
		if (ry == 0) {
			u = 2; //down
		} else if (ry == h - 1) {
			u = 0; // up
		} else {
			u = 1; //middle
		}
		sprite.x = 320 + t * TILE_SIZE;
		sprite.y = 0 + u * TILE_SIZE;
		if (blink) {
			sprite.y += 3 * TILE_SIZE;
		}
		return sprite;
	}

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
	auto garbageBlocks = _board.getGarbageBlocks();
	for (auto it = garbageBlocks.begin(); it != garbageBlocks.end(); ++it) {
		for (int y = it->getY() - (it->getH() - 1); y <= it->getY(); ++y) {
			for (int x = it->getX() + (it->getW() - 1); x >= it->getX(); --x) {
				SDL_Rect pos;
				pos.h = TILE_SIZE;
				pos.w = TILE_SIZE;
				pos.x = x * TILE_SIZE;
				pos.y = (BOARD_HEIGHT - (y + 1) * TILE_SIZE)
						- _board.getStackOffset();

				//lower right corner is 0,0
				int rx = (it->getX() + (it->getW() - 1)) - x;
				int ry = y - (it->getY() - (it->getH() - 1));
				int size = it->getW() * it->getH();

				double time = it->getTransformationTimer()
						- it->getAnimationStart();
				int ticks = it->getW() * it->getH()
						* Board::GARBAGE_TRANSFORM_STEP_TICKS;
				double block = it->getW() * ry + rx;
				if (it->getTransformationTimer()
						<= Board::BASE_TRANSFORMATION_TICKS) {
					//normal
					SDL_Rect sprite = getGarbageBlockSprite(rx, ry, *it);
					SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
				} else if (time / ticks >= block / size) { //block has been revealed
					if (ry == 0) {
						//draw revealed block
						SDL_Rect sprite = getBlockSprite(
								it->getBufferRow(it->getW() - rx - 1));
						SDL_SetTextureColorMod(_spriteSheet, 0x50, 0x50, 0x50);
						SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite,
								&pos);
						SDL_SetTextureColorMod(_spriteSheet, 0xFF, 0xFF, 0xFF);
						continue;
					} else {
						//normal
						SDL_Rect sprite = getGarbageBlockSprite(rx, ry, *it);
						SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite,
								&pos);
					}
				} else {
					//transforming block
					SDL_Rect sprite = { 480, 0, TILE_SIZE, TILE_SIZE };
					SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
				}
			}
		}

	}
}

void BoardRenderer::drawCursor() {
	SDL_Rect sprite = { 224, 64, 76, 44 };
	if (_board.getTicksRun() % (2 * CURSOR_ANIM_TICKS) >= CURSOR_ANIM_TICKS) {
		sprite.y += 44;
	}
	SDL_Rect pos = { (_board.getCursorX() * TILE_SIZE) - 6, (BOARD_HEIGHT
			- (_board.getCursorY() + 1) * TILE_SIZE) - _board.getStackOffset()
			- 6, 76, 44 };
	SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
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
	SDL_DestroyTexture(_readyText);
	SDL_DestroyTexture(_3Text);
	SDL_DestroyTexture(_2Text);
	SDL_DestroyTexture(_1Text);
	SDL_DestroyTexture(_goText);
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
