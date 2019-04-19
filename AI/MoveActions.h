
/* 
 * File:   MoveActions.h
 * Author: axel
 *
 * Created on October 2, 2016, 3:24 PM
 */

#ifndef MOVEACTIONS_H
#define MOVEACTIONS_H

struct BlockMoveAction {
  int x, y, dx, dy;
};

struct CursorMoveAction {
  int x, y;
};

#endif /* MOVEACTIONS_H */

