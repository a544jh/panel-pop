/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "GameRenderer.h"

#include <stddef.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <sstream>
#include <iomanip>

#include "../SDLContext.h"

GameRenderer::GameRenderer(Game& game) :
		_game(game), _boardRenderer(_game._board), _boardRenderer2(
				_game._board2), _gbqr(_game._board), _gbqr2(_game._board2) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);
	_bg = _SDLContext.makeTextureFromImage("assets/bg1.png");
	_2pbg = _SDLContext.makeTextureFromImage("assets/2p.png");
	SDL_SetTextureBlendMode(_2pbg, SDL_BLENDMODE_BLEND);
}

void GameRenderer::tick() {
	if (!_game.isPaused()) {
		_boardRenderer.tick();
		_boardRenderer2.tick();
	}
}

SDL_Texture* GameRenderer::renderGame() {

	SDL_Texture* boardTexture = _boardRenderer.renderBoard();
	SDL_Texture* boardTexture2 = _boardRenderer2.renderBoard();
	SDL_Texture* gbq = _gbqr.renderQueue();
	SDL_Texture* gbq2 = _gbqr2.renderQueue();

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	SDL_RenderCopy(_SDLRenderer, _bg, NULL, NULL);
	SDL_RenderCopy(_SDLRenderer, _2pbg, NULL, NULL);

	SDL_Rect rekt;
	rekt.x = 58;
	rekt.y = 43;
	rekt.w = _boardRenderer.BOARD_WIDTH;
	rekt.h = _boardRenderer.BOARD_HEIGHT;
	SDL_RenderCopy(_SDLRenderer, boardTexture, NULL, &rekt);
	SDL_Rect gbqp = { 258, 307, 38, 120 };
	SDL_RenderCopy(_SDLRenderer, gbq, NULL, &gbqp);

	rekt.x = 390;
	gbqp.x = 344;

	SDL_RenderCopy(_SDLRenderer, boardTexture2, NULL, &rekt);
	SDL_RenderCopy(_SDLRenderer, gbq2, NULL, &gbqp);

	renderStatsText();
	renderMatchPoints();

	if (_game.isPaused()) {
		SDL_SetTextureColorMod(_texture, 0x50, 0x50, 0x50);
	} else {
		SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
	}

	return _texture;
}

void GameRenderer::renderStatsText() {
	//time
	std::ostringstream os;
	uint32_t time;
	if (_game._board.getState() == Board::COUNTDOWN) {
		time = 0;
	} else {
		time = _game.getTime() - Board::COUNTDOWN_MS;
	}
	os << (time / 1000 / 60) << "\u2019" << std::setw(2) << std::setfill('0')
			<< ((time / 1000) % 60) << "\u201d";
	SDL_Texture* timeTexture = _SDLContext.makeTextureFromFont(os.str(), { 255,
			255, 255 }, _SDLContext._psFont);
	SDL_Rect pos = { 280, 218, 0, 0 };
	SDL_QueryTexture(timeTexture, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(_SDLRenderer, timeTexture, NULL, &pos);
	SDL_DestroyTexture(timeTexture);
	//points
	os.str("");
	os.clear();
	os << "\u25c0" << std::setw(2) << std::setfill('0') << _game.getP1Points()
			<< ' ' << std::setw(2) << std::setfill('0') << _game.getP2Points()
			<< "\u25b6";
	SDL_Texture* pointsTexture = _SDLContext.makeTextureFromFont(os.str(), {
			255, 255, 255 }, _SDLContext._psFont);
	pos = {264,180,0,0};
	SDL_QueryTexture(pointsTexture, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(_SDLRenderer, pointsTexture, NULL, &pos);
	SDL_DestroyTexture(pointsTexture);
}

void GameRenderer::renderMatchPoints() {
	//p1
	SDL_Rect sprite = { 0, 361, 21, 21 };
	SDL_Rect pos = { 259, 35, 21, 21 };
	for (int i = 0; i < Game::MATCH_POINTS; ++i) {
		if (_game.getP1MatchPoints() >= Game::MATCH_POINTS - i) {
			sprite.x = 21;
		} else {
			sprite.x = 0;
		}
		SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
		pos.y += 23;
	}
	//p2
	pos = {359,35,21,21};
	for (int i = 0; i < Game::MATCH_POINTS; ++i) {
		if (_game.getP2MatchPoints() >= Game::MATCH_POINTS - i) {
			sprite.x = 21;
		} else {
			sprite.x = 0;
		}
		SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
		pos.y += 23;
	}
}

GameRenderer::~GameRenderer() {
	SDL_DestroyTexture(_bg);
	SDL_DestroyTexture(_2pbg);
}

