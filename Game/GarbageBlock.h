/*
 * GarbageBlock.h
 *
 *  Created on: Aug 17, 2015
 *      Author: axel
 */

#ifndef GARBAGEBLOCK_H_
#define GARBAGEBLOCK_H_

#include "Block.h"


//need to put this here because of circular dependencies...
enum class GarbageBlockType {
	NORMAL, GRAY
};

enum class GarbageBlockState {
	NORMAL, TRIGGERED
};

class GarbageBlock {

	friend class Board;

public:
	GarbageBlock(int,int,int,int, GarbageBlockType);
	virtual ~GarbageBlock();

	int getH() const {
		return _h;
	}

	int getW() const {
		return _w;
	}

	int getX() const {
		return _x;
	}

	int getY() const {
		return _y;
	}

	GarbageBlockState getState() const {
		return _state;
	}

private:
	int _x, _y; //coordinates of upper-right corner;
	int _w, _h;
	GarbageBlockType _type;
	GarbageBlockState _state;
	Block _bufferRow[6];
	int _transformationTicks; //ticks it takes for block to transform

	void fillBufferRow();
	void trigger();
};

#endif /* GARBAGEBLOCK_H_ */
