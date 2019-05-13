/*
 * Block.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Block.h"
#include <stdlib.h>

Block::Block() :
    _state(NORMAL),
    _color(COUNT),
    _falling(false),
    _floatTimer(0),
    _swapTimer(0),
    _gbFallChain(false),
    _explOrder(0),
    _explosionTicks(0),
    _explosionAnimTicks(0),
    _explosionTimer(0) {

}

Block::~Block() {
}

BlockColor Block::getRandomColor(std::list<int> colors) {
    auto it = colors.begin();

    int r = (rand() % colors.size());
    for (int k = 0; k < r; k++) {
        it++;
    }
    return static_cast<BlockColor>(*it);
}
