/*
 * GarbageBlock.h
 *
 *  Created on: Aug 17, 2015
 *      Author: axel
 */

#ifndef GARBAGEBLOCK_H_
#define GARBAGEBLOCK_H_

#include "Block.h"
#include "Board.h"

class GarbageBlock {
public:

	enum Type {
		NORMAL, GRAY
	};

	GarbageBlock(int,int,int,int, Type);
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

private:
	int _x, _y; //coordinates of upper-right corner;
	int _w, _h;
	Type _type;
	Block _bufferRow[Board::BOARD_WIDTH];
	void fillBufferRow();
};

#endif /* GARBAGEBLOCK_H_ */
