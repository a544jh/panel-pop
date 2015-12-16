/*
 * Block.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Block.h"
#include <stdlib.h>

Block::Block() :
		_state(NORMAL), _falling(false), _explosionTicks(0), _explosionAnimTicks(
				0), _explosionTimer(0), _color(COUNT), _chain(false), _floatTimer(
				0), _swapTimer(0) {
	// TODO Auto-generated constructor stub

}

Block::~Block() {
	// TODO Auto-generated destructor stub
}

BlockColor Block::getRandomColor(std::list<int> colors) {
	auto it = colors.begin();

	int r = (rand() % colors.size());
	for (int k = 0; k < r; k++) {
		it++;
	}
	return static_cast<BlockColor>(*it);
}
