/*
 * BasicStrategy.cpp
 */

#include "BasicStrategy.h"

#include "../Game/Board.h"
#include "BoardScanner.h"

BasicStrategy::BasicStrategy() {
}

const std::string &BasicStrategy::name() const {
  static const std::string kName = "Basic";
  return kName;
}

const std::string &BasicStrategy::key() const {
  static const std::string kKey = "basic";
  return kKey;
}

void BasicStrategy::cursorPath(int curX, int curY, int x, int y,
                               std::vector<AIStrategyAction> &out) {
  for (int i = curY; i < y; ++i) {
    out.push_back(AI_UP);
  }
  for (int i = curY; i > y; --i) {
    out.push_back(AI_DOWN);
  }
  for (int i = curX; i < x; ++i) {
    out.push_back(AI_RIGHT);
  }
  for (int i = curX; i > x; --i) {
    out.push_back(AI_LEFT);
  }
}

void BasicStrategy::blockMove(int x, int y, int dx, std::vector<AIStrategyAction> &out,
                              int &curX, int &curY) {
  if (x < 0 || dx < 0) {
    return;
  }
  if (dx > x) { // move right: swap the pair at (x, y) and walk right
    cursorPath(curX, curY, x, y, out);
    for (int i = 0; i < dx - x; ++i) {
      out.push_back(AI_SWAP);
      out.push_back(AI_RIGHT);
    }
    curX = dx;
    curY = y;
  } else if (dx < x) { // move left: the cursor sits one column further left
    cursorPath(curX, curY, x - 1, y, out);
    for (int i = 0; i < x - dx; ++i) {
      out.push_back(AI_SWAP);
      out.push_back(AI_LEFT);
    }
    curX = dx - 1;
    curY = y;
  }
}

std::vector<AIStrategyAction> BasicStrategy::plan(const Board &board) {
  std::vector<AIStrategyAction> actions;

  BoardScanner scanner(board);
  BoardScanner::VerticalMatch vertMatch = scanner.findVerticalMatch();
  BlockMoveAction flatteningMove = scanner.findStackFlatteningMove();

  int curX = board.getCursorX();
  int curY = board.getCursorY();

  if (board.isPanic() && flatteningMove.y != 0) {
    blockMove(flatteningMove.x, flatteningMove.y, flatteningMove.dx, actions, curX, curY);
    return actions;
  }

  if (!vertMatch.found) {
    actions.push_back(AI_RAISE);
    return actions;
  }

  int firstCol = scanner.findColorCol(vertMatch.color, vertMatch.topRow);
  int firstRow = vertMatch.topRow - 1;
  int alt = 0;
  for (int i = 0; i <= firstRow - vertMatch.bottomRow; ++i) {
    int altRow = (i % 2 == 0) ? firstRow - alt : vertMatch.bottomRow + alt++;
    int col = scanner.findColorCol(vertMatch.color, altRow);
    blockMove(col, altRow, firstCol, actions, curX, curY);
  }

  return actions;
}
