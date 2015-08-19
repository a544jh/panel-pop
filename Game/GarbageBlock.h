/*
 * GarbageBlock.h
 *
 *  Created on: Aug 17, 2015
 *      Author: axel
 */

#ifndef GARBAGEBLOCK_H_
#define GARBAGEBLOCK_H_

class GarbageBlock {
public:
	GarbageBlock();
	virtual ~GarbageBlock();
private:
	int x, y; //coordinates of upper-right corner;
	int w, h;
};

#endif /* GARBAGEBLOCK_H_ */
