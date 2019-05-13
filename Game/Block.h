/*
 * Block.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <list>

enum BlockState {
  NORMAL, FLOATING, MATCHED, EXPLODING, SWAPPING_LEFT, SWAPPING_RIGHT
};

enum BlockColor {
  PURPLE, YELLOW, RED, CYAN, GREEN,
  COUNT
};

class Block {
 public:
  Block();
  virtual ~Block();

  static BlockColor getRandomColor(std::list<int>);

  BlockState _state;
  BlockColor _color;

  bool _falling;//to prevent block from getting matched in mid-air
  int _floatTimer;//the blocks floats for a while after getting swapped
  int _swapTimer;//for swap delay and animation
  bool _gbFallChain; //falling block from transformed garbage, to trigger chains
  int _explOrder; //to determine sound effect

  int _explosionTicks; //ticks it takes for block to explode
  int _explosionAnimTicks; //ticks it takes for the explosion animation to complete
  int _explosionTimer; //gets increased every tick when block is exploding
};

#endif /* BLOCK_H_ */
