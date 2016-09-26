
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
    if (_board.getTicksRun() % 4) {
        _inputQueue.push(DOWN);
    }
    if (_board.getTicksRun() + 2 % 4) {
        _inputQueue.push(UP);
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
            _board.inputSwapBlocks() :
            break;
    }

}

AIBoardController::~AIBoardController() {
}

