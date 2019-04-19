/*
 * GbQueueRenderer.cpp
 *
 *  Created on: 3.1.2016
 *      Author: axelw
 */

#include "GbQueueRenderer.h"

GbQueueRenderer::GbQueueRenderer(Board &b) :
    _board(b) {
    _texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 38, 120);
    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
}

SDL_Texture *GbQueueRenderer::renderQueue() {
    SDL_SetRenderTarget(_SDLRenderer, _texture);
    SDL_SetRenderDrawColor(_SDLRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(_SDLRenderer);
    SDL_Rect pos = {0, 120, 38, 8};
    for (auto it = _board.getGarbageQueue().begin();
         it != _board.getGarbageQueue().end(); ++it) {
        if (it->spawnTimer <= 0) {
            SDL_SetTextureColorMod(_spriteSheet, 0x50, 0x50, 0x50);
        } else {
            SDL_SetTextureColorMod(_spriteSheet, 0xFF, 0xFF, 0xFF);
        }
        SDL_Rect sprite = {0, 305, 38, 8};
        if (!it->fullWidth || (it->fullWidth && it->size == 1)) {
            pos.y -= 8;
            if (it->fullWidth) {
                sprite.y -= (3 - 6) * 8;
            } else {
                sprite.y -= (3 - it->size) * 8;
            }
            SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
            pos.y -= 2;
        } else if (it->fullWidth && it->size > 1) {
            pos.y -= 14;
            pos.h = 14;
            sprite = {0, 337, 38, 14};
            SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
            pos.y += 2;
            pos.x += 12;
            pos.w = 6;
            pos.h = 10;
            sprite = {0, 351, 6, 10};
            SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos); //x
            //digits
            pos.x += 6;
            pos.w = 7;
            sprite.w = 7;
            if (it->size < 10) {
                sprite.x = 5 + (it->size - 1) * 7;
                SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
            } else if (it->size == 11) {
                sprite.x = 5 + 0 * 7;
                SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
                pos.x += 7;
                SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
            } else if (it->size == 12) {
                sprite.x = 5 + 0 * 7;
                SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
                pos.x += 7;
                sprite.x = 5 + 1 * 7;
                SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
            }

            pos.x = 0;
            pos.y -= 2 + 2;
            pos.h = 8;
            pos.w = 38;

        }
    }
    SDL_SetTextureColorMod(_spriteSheet, 0xFF, 0xFF, 0xFF);
    return _texture;
}

GbQueueRenderer::~GbQueueRenderer() {
}

