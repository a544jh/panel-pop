
#include <stdexcept>

#include "AIBoardController.h"

AIBoardController::AIBoardController(Board& board) :
BoardController(board),
_scanner(board) {
}

void AIBoardController::tick() {
    if (!_inputQueue.empty()) {
        doInput(_inputQueue.front());
        _inputQueue.pop();
    } else if
        (!_cursorQueue.empty()) {
        CursorMoveAction move = _cursorQueue.front();
        doCursorMove(move.x, move.y);
        _cursorQueue.pop();
    } else if (!_blockMoveQueue.empty()) {
        BlockMoveAction move = _blockMoveQueue.front();
        doBlockMove(move.x, move.y, move.dx, move.dy);
        _blockMoveQueue.pop();
    } else {
        basicVerticalmatch();
    }
}

void AIBoardController::doInput(InputAction action) {
    switch (action) {
        case (UP):
            _board.inputMoveCursor(Direction::UP);
            break;
        case (RIGHT):
            _board.inputMoveCursor(Direction::RIGHT);
            break;
        case (DOWN):
            _board.inputMoveCursor(Direction::DOWN);
            break;
        case (LEFT):
            _board.inputMoveCursor(Direction::LEFT);
            break;
        case (SWAP):
            _board.inputSwapBlocks();
            break;
        case (WAIT):
            break;
    }

}

void AIBoardController::doBlockMove(int x, int y, int dx, int dy) {
    if (dy > y) {
        throw std::invalid_argument("Can't move block upwards");
    }
    if (dx > x) { //move right
        doCursorMove(x, y);
        for (int i = 0; i < dx - x; i++) {
            _inputQueue.push(SWAP);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(RIGHT);

        }
    }
    if (dx < x) { //move left
        doCursorMove(x - 1, y);
        for (int i = 0; i < x - dx; i++) {
            _inputQueue.push(SWAP);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(LEFT);
        }
    }
}

void AIBoardController::doCursorMove(int x, int y) {
    int curX = _board.getCursorX();
    int curY = _board.getCursorY();

    if (x > curX) {
        for (int i = 0; i < x - curX; i++) {
            _inputQueue.push(RIGHT);
        }
    }
    if (x < curX) {
        for (int i = 0; i < curX - x; i++) {
            _inputQueue.push(LEFT);
        }
    }
    if (y > curY) {
        for (int i = 0; i < y - curY; i++) {
            _inputQueue.push(UP);
        }
    }
    if (y < curY) {
        for (int i = 0; i < curY - y; i++) {
            _inputQueue.push(DOWN);
        }
    }

}

void AIBoardController::basicVerticalmatch() {
    BoardScanner::VerticalMatch match = _scanner.findVerticalMatch();
    int firstCol = _scanner.findColorCol(match.color, match.bottomRow);
    for (int row = match.bottomRow + 1; row <= match.topRow; ++row) {
        int col = _scanner.findColorCol(match.color, row);
        BlockMoveAction action = {col, row, firstCol, row};
        _blockMoveQueue.push(BlockMoveAction(action));
    }
}

AIBoardController::~AIBoardController() {
}

