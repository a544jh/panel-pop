/*
 * Block.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Block.h"

Block::Block() :
		_state(NORMAL), _animBlinkState(false), _falling(false), _explosionTicks(
				0), _explosionAnimTicks(0), _explosionTimer(0), _color(COUNT), _noChain(
				false) {
	// TODO Auto-generated constructor stub

}

Block::~Block() {
	// TODO Auto-generated destructor stub
}

