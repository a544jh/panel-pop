/*
 * ComboPopup.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: axel
 */

#include "ComboPopup.h"

ComboPopup::ComboPopup() {

}

ComboPopup::ComboPopup(int x, int y, int value, int lifetime) :
    Popup(x, y, value, lifetime) {
}

void ComboPopup::render() {
    SDL_Rect dst;
    dst.x = _x;
    dst.y = _y;
    dst.w = 5;
    dst.h = 27;
    SDL_Rect sheet;
    sheet.x = 12;
    sheet.y = 278;
    sheet.w = 5;
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

ComboPopup::~ComboPopup() {
}

