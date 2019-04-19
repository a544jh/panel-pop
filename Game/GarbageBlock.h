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
  NORMAL, TRIGGERED, TRANSFORMING
};

class GarbageBlock {

  friend class Board;

 public:
  GarbageBlock(int, int, int, int, GarbageBlockType);
  virtual ~GarbageBlock();

  const Block &getBufferRow(int) const;
  int getH() const;
  GarbageBlockState getState() const;
  int getTransformationTicks() const;
  int getTransformationTimer() const;
  int getAnimationStart() const;
  GarbageBlockType getType() const;
  int getW() const;
  int getX() const;
  int getY() const;

  bool isInitialFall() const {
      return _initialFall;
  }

 private:
  int _x, _y; //coordinates of upper-right corner;
  int _w, _h;
  GarbageBlockType _type;
  GarbageBlockState _state;
  Block _bufferRow[6];
  int _transformationTicks; //ticks it takes for block to transform
  int _transformationTimer;
  int _animationStart;
  bool _falling;
  bool _initialFall;
  int _explOrder;

  void fillBufferRow();
  void trigger();
};

#endif /* GARBAGEBLOCK_H_ */
