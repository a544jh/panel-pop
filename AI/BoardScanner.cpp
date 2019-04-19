
/* 
 * File:   BoardScanner.cpp
 * Author: axel
 * 
 * Created on September 30, 2016, 10:22 PM
 */

#include "BoardScanner.h"
#include <iostream>

BoardScanner::BoardScanner(Board &board) :
    _board(board) {
}

BoardScanner::RowColors BoardScanner::countRowColors() {

    BoardScanner::RowColors counts;

    for (int row = 0; row < Board::BOARD_HEIGHT; ++row) {
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            Board::Tile tile = _board.getTile(row, col);
            if (tile.type == TileType::BLOCK && tile.b._state == BlockState::NORMAL) {
                BlockColor color = tile.b._color;
                counts[row][color] = counts[row][color] + 1;
            }
        }
    }
    return counts;
}

BoardScanner::ColorCounts BoardScanner::countColorsOn(int row, int startCol, int endCol) {
    BoardScanner::ColorCounts counts;
    for (int col = startCol; col <= endCol; ++col) {
        Board::Tile tile = _board.getTile(row, col);
        if (tile.type == TileType::BLOCK && tile.b._state == BlockState::NORMAL) {
            BlockColor color = tile.b._color;
            counts[color] = counts[color] + 1;
        }
    }
    return counts;
}

BoardScanner::VerticalMatch BoardScanner::findVerticalMatch() {
    RowColors rowColors = countRowColors();
    for (int colorInt = 0; colorInt < BlockColor::COUNT; ++colorInt) {
        BlockColor color = static_cast<BlockColor> (colorInt);
        int topRow = Board::BOARD_HEIGHT - 1;
        int sameColorFound = 0;
        for (int row = Board::BOARD_HEIGHT - 1; row >= 0; --row) {
            //std::cout << "Color " << color << " on row " << row << " " << rowColors[row][color] << " times\n";
            if (rowColors[row][color] > 0) {
                sameColorFound++;
            } else {
                if (sameColorFound >= 3) {
                    //TODO: maybe find max...
                    VerticalMatch match = {true, color, row + 1, topRow};
                    return match;
                }
                topRow = row - 1;
                sameColorFound = 0;
            }
        }
        if (sameColorFound >= 3) {
            VerticalMatch match = {true, color, 0, topRow};
            return match;
        }
    }
    VerticalMatch match = {false};
    return match;
}

int BoardScanner::findColorCol(BlockColor color, int row) {
    return findColorOn(color, row, 0, Board::BOARD_WIDTH - 1);
}

int BoardScanner::findColorOn(BlockColor color, int row, int startCol, int endCol) {
    for (int col = startCol; col <= endCol; ++col) {
        Board::Tile tile = _board.getTile(row, col);
        if (tile.type == TileType::BLOCK && tile.b._color == color) {
            return col;
        }
    }
    return -1;
}

BlockMoveAction BoardScanner::findStackFlatteningMove() {
    for (int row = Board::BOARD_HEIGHT - 1; row >= 1; --row) {
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            //can be moved left
            if (col > 0 && _board.getTile(row, col).type == BLOCK) {
                for (int dcol = col - 1; dcol >= 0; --dcol) {
                    if (_board.getTile(row, dcol).type != AIR) break;
                    if (_board.getTile(row - 1, dcol).type == AIR) {
                        BlockMoveAction action = {col, row, dcol, row};
                        return action;
                    }
                }
            }
            //can be moved right
            if (col < Board::BOARD_WIDTH - 1 && _board.getTile(row, col).type == BLOCK) {
                for (int dcol = col + 1; dcol < Board::BOARD_WIDTH; ++dcol) {
                    if (_board.getTile(row, dcol).type != AIR) break;
                    if (_board.getTile(row - 1, dcol).type == AIR) {
                        BlockMoveAction action = {col, row, dcol, row};
                        return action;
                    }
                }
            }
        }
    }
    BlockMoveAction action = {0, 0, 0, 0};
    return action;
}

BoardScanner::ChainOffsetArea BoardScanner::findChainOffsetArea() {
    ChainOffsetArea area;
    area = {false, 0, 0, 0, 0};

    for (int row = 0; row < Board::BOARD_HEIGHT; ++row) {
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            Board::Tile tile = _board.getTile(row, col);
            if (tile.type == TileType::BLOCK
                && tile.b._state == BlockState::EXPLODING) {
                area.found = true;
                area.col = col;
                area.row = row;
                goto findSize;
            }
        }
    }
    if (!area.found) {
        return area;
    }
    findSize:
    for (int row = area.row; row < Board::BOARD_HEIGHT; ++row) {
        Board::Tile tile = _board.getTile(row, area.col);
        if (tile.type == TileType::BLOCK
            && tile.b._state == BlockState::EXPLODING) {
            ++area.offset;
        } else {
            break;
        }
    }
    for (int col = 0; area.col < Board::BOARD_WIDTH; ++col) {
        Board::Tile tile = _board.getTile(area.row, col);
        if (tile.type == TileType::BLOCK
            && tile.b._state == BlockState::EXPLODING) {
            ++area.width;
        } else {
            break;
        }
    }
    return area;
}

BoardScanner::ChainMatch BoardScanner::findChainMatch() {
    ChainMatch match;
    ChainOffsetArea area = findChainOffsetArea();
    if (!area.found) {
        match.found = false;
        return match;
    }

    int row = 0;
    BoardScanner::ColorCounts lowerCount;
    BoardScanner::ColorCounts offsetCount = countColorsOn(row + area.offset, 0, Board::BOARD_WIDTH - 1);;

    auto countColors = [&](int matchCol, Direction side) {
      for (int colorInt = 0; colorInt < BlockColor::COUNT; ++colorInt) {
          BlockColor color = static_cast<BlockColor> (colorInt);
          if ((area.width == 1 && lowerCount[color] >= 2 && offsetCount[color] > 0)
              || (area.width > 1 && lowerCount[color] > 0 && offsetCount[color] >= 2)) {
              match.found = true;
              match.color = color;
              match.side = side;
              match.col = matchCol;
              match.row = row;
              match.offsetRow = row + area.offset;
              return match;
          }
      }

      match.found = false;
      return match;
    };

    if (area.col > 0) {
        for (row = area.row; row + area.offset < Board::BOARD_HEIGHT; ++row) {
            lowerCount = countColorsOn(row, 0, area.col - 1);
            return countColors(area.col - 1, LEFT);
        }
    }
    if (area.col + area.width - 1 < Board::BOARD_WIDTH) {
        for (row = area.row; row + area.offset < Board::BOARD_HEIGHT; ++row) {
            lowerCount = countColorsOn(row, area.col + area.width - 1, Board::BOARD_WIDTH - 1);
            return countColors(area.col + area.width - 1, RIGHT);
        }
    }

    match.found = false;
    return match;
}

BoardScanner::~BoardScanner() {
}

