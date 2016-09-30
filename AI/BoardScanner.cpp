
/* 
 * File:   BoardScanner.cpp
 * Author: axel
 * 
 * Created on September 30, 2016, 10:22 PM
 */

#include "BoardScanner.h"

BoardScanner::BoardScanner(Board& board) :
_board(board) {
}

BoardScanner::rowColors BoardScanner::countRowColors() {

    BoardScanner::rowColors counts;

    for (int row = 0; row < Board::BOARD_HEIGHT; ++row) {
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            Board::Tile tile = _board.getTile(row, col);
            if (tile.type == TileType::BLOCK) {
                BlockColor color = tile.b._color;
                counts[row][color] = counts[row][color] + 1;
            }
        }
    }
    return counts;
}

BoardScanner::VerticalMatch BoardScanner::findVerticalMatch() {
    rowColors rowColors = countRowColors();
    for (int colorInt = 0; colorInt < BlockColor::COUNT; ++colorInt) {
        BlockColor color = static_cast<BlockColor> (colorInt);
        int bottomRow = 0;
        int matched = 1;
        for (int row = 0; row < Board::BOARD_HEIGHT; ++row) {
            if (rowColors[row][color] > 0) {
                matched++;
            } else {
                if (matched > 3) {
                    //TODO: maybe find max...
                    VerticalMatch match = {color, bottomRow, bottomRow + matched};
                    return match;
                }
                bottomRow = row;
                matched = 1;
            }
        }
    }
    VerticalMatch match = {BlockColor::CYAN, 0, 0};
    return match; //TODO: fix...
}

int BoardScanner::findColorCol(BlockColor color, int row) {
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        Board::Tile tile = _board.getTile(row, col);
        if (tile.type == TileType::BLOCK && tile.b._color == color) {
            return col;
        }
    }
   return 0; //TODO: fix...
}

BoardScanner::~BoardScanner() {
}

