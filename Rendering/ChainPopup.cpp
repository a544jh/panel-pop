/*
 * ChainPanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "ChainPopup.h"

ChainPopup::ChainPopup() {
}

ChainPopup::ChainPopup(int x, int y, int value, int lifetime) :
    Popup(x, y, value, lifetime) {
}

void ChainPopup::render() {
    SDL_Rect dst;
    dst.x = _x;
    dst.y = _y;
    dst.w = 17;
    dst.h = 27;
    SDL_Rect sheet;
    sheet.x = 0;
    sheet.y = 248;
    sheet.w = 17;
    sheet.h = 27;
    SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &dst);
    dst.x += sheet.w;

    sheet.w = 12;
    dst.w = 12;
    for (char &c : _digits) {
        int digit = c - '0';
        if (digit == 0) {
            sheet.x = 145;
        } else {
            sheet.x = 19 + (digit - 1) * 14;
        }
        SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &dst);
        dst.x += sheet.w;
    }

    sheet.x = 159;
    sheet.w = 5;
    dst.w = 5;
    SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sheet, &dst);
}

ChainPopup::~ChainPopup() {
}
