/*
 * GarbageBlock.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: axel
 */

#include "GarbageBlock.h"

GarbageBlock::GarbageBlock(int x, int y, int w, int h, GarbageBlockType t) :
		_x(x), _y(y), _w(w), _h(h), _type(t), _state(GarbageBlockState::NORMAL), _transformationTicks(
				0), _transformationTimer(0), _animationTicks(0), _falling(false) {
	fillBufferRow();
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

void GarbageBlock::trigger() {
	_state = GarbageBlockState::TRIGGERED;
}

void GarbageBlock::transform(int tt) {
	_state = GarbageBlockState::TRANSFORMING;
	_transformationTicks = tt;
}

const Block& GarbageBlock::getBufferRow(int x) const {
	return _bufferRow[x];
}

int GarbageBlock::getH() const {
	return _h;
}

GarbageBlockState GarbageBlock::getState() const {
	return _state;
}

int GarbageBlock::getTransformationTicks() const {
	return _transformationTicks;
}

int GarbageBlock::getTransformationTimer() const {
	return _transformationTimer;
}

GarbageBlockType GarbageBlock::getType() const {
	return _type;
}

int GarbageBlock::getW() const {
	return _w;
}

int GarbageBlock::getX() const {
	return _x;
}

int GarbageBlock::getY() const {
	return _y;
}

int GarbageBlock::getAnimationTicks() const {
	return _animationTicks;
}
