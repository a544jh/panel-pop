/*
 * GarbageBlock.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: axel
 */

#include "GarbageBlock.h"

GarbageBlock::GarbageBlock(int x, int y, int w, int h, GarbageBlockType t) :
		_x(x), _y(y), _w(w), _h(h), _type(t) {
	// TODO Auto-generated constructor stub

}

GarbageBlock::~GarbageBlock() {
	// TODO Auto-generated destructor stub
}


void GarbageBlock::fillBufferRow() {
	for (int i = 0; i < _w; i++) {
		std::list<int> colors;
		for (int k = 0; k < BlockColor::COUNT; k++) {
			colors.push_back(k);
		}
		colors.remove(_bufferRow[i - 1]._color);
		_bufferRow[i]._color = Block::getRandomColor(colors);
		_bufferRow[i]._chain = true;
	}
}
