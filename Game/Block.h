/*
 * Block.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BLOCK_H_
#define BLOCK_H_

enum BlockState {
	NORMAL, FLOATING, MATCHED, EXPLODING
};

enum BlockColor {
	PURPLE, YELLOW, RED, CYAN, GREEN,
	COUNT
};

class Block {
public:
	Block();
	virtual ~Block();

	BlockState _state;
	BlockColor _color;

	bool _falling;//to prevent block from getting matched in mid-air
	int _floatTimer;//the blocks floats for a while after getting swapped

	int _explosionTicks; //ticks it takes for block to explode
	int _explosionAnimTicks; //ticks it takes for the explosion animation to complete
	int _explosionTimer; //gets increased every tick when block is exploding
	bool _animBlinkState;//makes the block flicker
};

#endif /* BLOCK_H_ */
