/*
 * Block.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Block.h"

Block::Block() :
		_state(NORMAL), _falling(false), _explosionTicks(
				0), _explosionAnimTicks(0), _explosionTimer(0), _color(COUNT), _chain(
				false), _floatTimer(0), _swapTimer(0) {
	// TODO Auto-generated constructor stub

}

Block::~Block() {
	// TODO Auto-generated destructor stub
}

