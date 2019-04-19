/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

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
#include "VsGameRenderer.h"

VsGameRenderer::VsGameRenderer(VsGame &game) :
    GameRenderer(game),
    _game(game),
    _boardRenderer0(_game.getBoard(0)),
    _boardRenderer1(_game.getBoard(1)),
    _gbqr0(_game.getBoard(0)),
    _gbqr1(_game.getBoard(1)),
    _b0Shake(0),
    _b1Shake(0) {
    _texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 640, 480);
    _bg = _SDLContext.makeTextureFromImage("assets/bg1.png");
    _2pbg = _SDLContext.makeTextureFromImage("assets/2p.png");
    SDL_SetTextureBlendMode(_2pbg, SDL_BLENDMODE_BLEND);
}

void VsGameRenderer::tick() {
    if (!_game.isPaused()) {
        _boardRenderer0.tick();
        _boardRenderer1.tick();
        handleParticles();
        handleShake();
    }
}

SDL_Texture *VsGameRenderer::renderGame() {

    SDL_Texture *gbq = _gbqr0.renderQueue();
    SDL_Texture *gbq2 = _gbqr1.renderQueue();

    SDL_SetRenderTarget(_SDLRenderer, _texture);
    SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(_SDLRenderer);

    SDL_RenderCopy(_SDLRenderer, _bg, NULL, NULL);
    SDL_RenderCopy(_SDLRenderer, _2pbg, NULL, NULL);

    if (!_game.isPaused()) {

        renderBoard(0);
        renderBoard(1);
        SDL_Rect gbqp = {258, 307, 38, 120};
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
        _SDLContext.renderText("PUSH START", {255, 255, 255},
                               _SDLContext._fontSquare, 134, 342);
    }

    return _texture;
}

void VsGameRenderer::renderStatsText() {
//time

    renderGameTime(280, 218);

//points
    std::ostringstream os;
    os << "\u25c0" << std::setw(2) << std::setfill('0') << _game.getP1Points()
       << ' ' << std::setw(2) << std::setfill('0') << _game.getP2Points()
       << "\u25b6";
    _SDLContext.renderText(os.str(), {255, 255, 255}, _SDLContext._fontPs,
                           264, 180);
}

void VsGameRenderer::renderMatchPoints() {
//p1
    SDL_Rect sprite = {0, 361, 21, 21};
    SDL_Rect pos = {260, 35, 21, 21};
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
    pos = {359, 35, 21, 21};
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

VsGameRenderer::~VsGameRenderer() {
    SDL_DestroyTexture(_bg);
    SDL_DestroyTexture(_2pbg);
}

void VsGameRenderer::shakeBoard(int id, int duration) {
    if (id == 0) {
        _b0Shake = duration;
    } else if (id == 1) {
        _b1Shake = duration;
    }
}

void VsGameRenderer::handleShake() {
    if (_b0Shake > 0) {
        --_b0Shake;
    }
    if (_b1Shake > 0) {
        --_b1Shake;
    }
}

void VsGameRenderer::renderBoard(int id) {
    SDL_Rect pos;
    double shake;
    SDL_Texture *boardTexture;
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
    SDL_Rect src = {0, 0, BoardRenderer::BOARD_WIDTH,
                    BoardRenderer::BOARD_HEIGHT};

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
