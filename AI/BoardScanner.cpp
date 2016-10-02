
/* 
 * File:   BoardScanner.cpp
 * Author: axel
 * 
 * Created on September 30, 2016, 10:22 PM
 */

#include "BoardScanner.h"
#include <iostream>

BoardScanner::BoardScanner(Board& board) :
_board(board) {
}

BoardScanner::rowColors BoardScanner::countRowColors() {

    BoardScanner::rowColors counts;

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

BoardScanner::VerticalMatch BoardScanner::findVerticalMatch() {
    rowColors rowColors = countRowColors();
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
                topRow = row;
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
    for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
        Board::Tile tile = _board.getTile(row, col);
        if (tile.type == TileType::BLOCK && tile.b._color == color) {
            return col;
        }
    }
    return 0; //TODO: fix...
}

BlockMoveAction BoardScanner::findStackFlatteningMove() {
    for (int row = Board::BOARD_HEIGHT - 1; row >= 1; --row) {
        for (int col = 0; col < Board::BOARD_WIDTH; ++col) {
            //can be moved left
            if (col > 0 && _board.getTile(row, col - 1).type == AIR
                    && _board.getTile(row, col).type == BLOCK) {
                BlockMoveAction action = {col, row, 0, row};
                return action;
            }
            //can be moved right
            if (col < Board::BOARD_WIDTH - 1 && _board.getTile(row, col + 1).type == AIR
                    && _board.getTile(row, col).type == BLOCK) {
                BlockMoveAction action = {col, row, Board::BOARD_WIDTH, row};
                return action;
            }
        }
    }
    BlockMoveAction action = {0, 0, 0, 0};
    return action;
}

BoardScanner::~BoardScanner() {
}

