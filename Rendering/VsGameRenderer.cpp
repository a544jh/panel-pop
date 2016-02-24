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
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <math.h>

#include "../Game/Board.h"
#include "../Game/Game.h"
#include "../Game/VsGame.h"
#include "../SDLContext.h"

GameRenderer::GameRenderer(VsGame& game) :
				_game(game),
				_boardRenderer0(_game._board0),
				_boardRenderer1(_game._board1),
				_gbqr0(_game._board0),
				_gbqr1(_game._board1),
				_b0Shake(0),
				_b1Shake(0) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);
	_bg = _SDLContext.makeTextureFromImage("assets/bg1.png");
	_2pbg = _SDLContext.makeTextureFromImage("assets/2p.png");
	SDL_SetTextureBlendMode(_2pbg, SDL_BLENDMODE_BLEND);
}

void GameRenderer::tick() {
	if (!_game.isPaused()) {
		_boardRenderer0.tick();
		_boardRenderer1.tick();
		handleParticles();
		handleShake();
	}
}

SDL_Texture* GameRenderer::renderGame() {

	SDL_Texture* gbq = _gbqr0.renderQueue();
	SDL_Texture* gbq2 = _gbqr1.renderQueue();

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(_SDLRenderer);

	SDL_RenderCopy(_SDLRenderer, _bg, NULL, NULL);
	SDL_RenderCopy(_SDLRenderer, _2pbg, NULL, NULL);

	if (!_game.isPaused()) {

		renderBoard(0);
		renderBoard(1);
		SDL_Rect gbqp = { 258, 307, 38, 120 };
		SDL_RenderCopy(_SDLRenderer, gbq, NULL, &gbqp);
		gbqp.x = 344;
		SDL_RenderCopy(_SDLRenderer, gbq2, NULL, &gbqp);
		renderParticles();

	}
	renderStatsText();
	renderMatchPoints();

	if (_game.isPaused()) {
		SDL_SetTextureColorMod(_texture, 0x50, 0x50, 0x50);
		SDL_RenderCopy(_SDLRenderer, _texture, NULL, NULL);
		SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
		_game.getPauseMenu().render();
	} else {
		SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
	}

	//TODO: make this state more fancy
	if (_game.getState() == Game::State::ENDED
			&& SDL_GetTicks() % 1000 >= 500) {
		_SDLContext.renderText("PUSH START", { 255, 255, 255 },
				_SDLContext._fontSquare, 134, 342);
	}

	return _texture;
}

void GameRenderer::renderStatsText() {
//time
	std::ostringstream os;
	uint32_t time;
	if (_game._board0.getState() == Board::COUNTDOWN) {
		time = 0;
	} else {
		time = _game.getTime() - Board::COUNTDOWN_MS;
	}
	os << (time / 1000 / 60) << "\u2019" << std::setw(2) << std::setfill('0')
			<< ((time / 1000) % 60) << "\u201d";
	_SDLContext.renderText(os.str(), { 255, 255, 255 }, _SDLContext._fontPs,
			280, 218);
//points
	os.str("");
	os.clear();
	os << "\u25c0" << std::setw(2) << std::setfill('0') << _game.getP1Points()
			<< ' ' << std::setw(2) << std::setfill('0') << _game.getP2Points()
			<< "\u25b6";
	_SDLContext.renderText(os.str(), { 255, 255, 255 }, _SDLContext._fontPs,
			264, 180);
}

void GameRenderer::renderMatchPoints() {
//p1
	SDL_Rect sprite = { 0, 361, 21, 21 };
	SDL_Rect pos = { 260, 35, 21, 21 };
	for (int i = 0; i < VsGame::MATCH_POINTS; ++i) {
		if (_game.getP1MatchPoints() >= VsGame::MATCH_POINTS - i) {
			sprite.x = 21;
		} else {
			sprite.x = 0;
		}
		SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
		pos.y += 23;
	}
//p2
	pos = {359,35,21,21};
	for (int i = 0; i < VsGame::MATCH_POINTS; ++i) {
		if (_game.getP2MatchPoints() >= VsGame::MATCH_POINTS - i) {
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

void GameRenderer::addParticle(Particle* p) {
	_particles.push_back(p);
}

void GameRenderer::handleParticles() {
	auto it = _particles.begin();
	while (it != _particles.end()) {
		if ((*it)->_alive) {
			(*it)->tick();
			++it;
		} else {
			delete *it;
			it = _particles.erase(it);
		}
	}
}

void GameRenderer::renderParticles() {
	for (auto it = _particles.begin(); it != _particles.end(); ++it) {
		(*it)->render();
	}
}

void GameRenderer::shakeBoard(int id, int duration) {
	if (id == 0) {
		_b0Shake = duration;
	} else if (id == 1) {
		_b1Shake = duration;
	}
}

void GameRenderer::handleShake() {
	if (_b0Shake > 0) {
		--_b0Shake;
	}
	if (_b1Shake > 0) {
		--_b1Shake;
	}
}

void GameRenderer::renderBoard(int id) {
	SDL_Rect pos;
	double shake;
	SDL_Texture* boardTexture;
	pos.h = BoardRenderer::BOARD_HEIGHT;
	pos.w = BoardRenderer::BOARD_WIDTH;
	if (id == 0) {
		pos.x = BOARD0_X;
		pos.y = BOARD0_Y;
		shake = _b0Shake;
		boardTexture = _boardRenderer0.renderBoard();
	} else {
		pos.x = BOARD1_X;
		pos.y = BOARD1_Y;
		shake = _b1Shake;
		boardTexture = _boardRenderer1.renderBoard();
	}

	int shakeOffset = .5 * shake * sin(shake);
	SDL_Rect src = {0,0,BoardRenderer::BOARD_WIDTH,BoardRenderer::BOARD_HEIGHT};

	if (shakeOffset >= 0) {
		pos.y += shakeOffset;
		pos.h -= shakeOffset;
		src.h -= shakeOffset;
	} else {
		pos.h += shakeOffset;
		src.y -= shakeOffset;
		src.h += shakeOffset;
	}

	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_RenderCopy(_SDLRenderer, boardTexture, &src, &pos);
}
