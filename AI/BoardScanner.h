/*
 * File:   BoardScanner.h
 * Author: axel
 *
 * Created on September 30, 2016, 10:22 PM
 */

#ifndef BOARDSCANNER_H
#define BOARDSCANNER_H

#include <map>
#include <array>
#include "../Game/Board.h"
#include "MoveActions.h"

class BoardScanner {
 public:
  typedef std::map<BlockColor, unsigned int> ColorCounts;
  typedef std::array<ColorCounts, Board::BOARD_HEIGHT> RowColors;

  struct VerticalMatch {
    bool found;
    BlockColor color;
    int bottomRow;
    int topRow;
  };

  struct ChainOffsetArea {
    bool found;
    int col, row, //bottom left
        width, offset;
  };

  struct ChainMatch {
    bool found;
    BlockColor color;
    int row, //bottom of gap
        offsetRow, //above gap
        col; //next to gap
    Direction side; //side of gap
  };

  BoardScanner(Board &);

  RowColors countRowColors();
  ColorCounts countColorsOn(int row, int startCol, int endCol);
  VerticalMatch findVerticalMatch();
  int findColorCol(BlockColor color, int row);
  int findColorOn(BlockColor color, int row, int startCol, int endCol);

  BlockMoveAction findStackFlatteningMove();
  ChainOffsetArea findChainOffsetArea();
  ChainMatch findChainMatch();
  virtual ~BoardScanner();
 private:
  Board &_board;

};

#endif /* BOARDSCANNER_H */
