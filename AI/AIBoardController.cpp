
#include <stdexcept>

#include "AIBoardController.h"

AIBoardController::AIBoardController(Board& board) :
BoardController(board) {
}

void AIBoardController::tick() {
    if (!_inputQueue.empty()) {
        doInput(_inputQueue.front());
        _inputQueue.pop();
    }
        //test logic
    else if (true) {
        moveBlock(0, 0, 5, 0);
        //moveCursorTo(0,0);
    } else if (_board.getTicksRun() % 500 == 250) {
        //moveCursorTo(10,5);
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
    }

}

void AIBoardController::moveBlock(int x, int y, int dx, int dy) {
    if (dy > y) {
        throw std::invalid_argument("Can't move block upwards");
    }
    if (dx > x) { //move right
        moveCursorTo(x, y);
        for (int i = 0; i < dx - x; i++) {
            _inputQueue.push(SWAP);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            moveCursorTo(x + i + 1, y);
        }
    }
    if (dx < x) { //move left
        moveCursorTo(x - 1, y);
        for (int i = 0; i < x - dx; i++) {
            _inputQueue.push(SWAP);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            _inputQueue.push(WAIT);
            moveCursorTo(x - i, y);
        }
    }
}

void AIBoardController::moveCursorTo(int x, int y) {
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

AIBoardController::~AIBoardController() {
}

