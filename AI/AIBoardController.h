
#ifndef AIBOARDCONTROLLER_H
#define AIBOARDCONTROLLER_H

#include<queue>
#include"../Game/BoardController.h"
#include"MoveActions.h"

#include "BoardScanner.h"

class AIBoardController : public BoardController {
 public:

  enum InputAction {
    UP, RIGHT, DOWN, LEFT, SWAP, RAISE, WAIT
  };

  AIBoardController(Board &);

  void tick() override;

  virtual ~AIBoardController();
 private:
  BoardScanner _scanner;

  std::queue<InputAction> _inputQueue;
  std::queue<CursorMoveAction> _cursorQueue;
  std::queue<BlockMoveAction> _blockMoveQueue;

  void doInput(InputAction);
  void doCursorMove(int x, int y);
  void doBlockMove(int x, int y, int dx, int dy);
  void doVerticalMatch(BoardScanner::VerticalMatch match);
  void doChainMatch(BoardScanner::ChainMatch match);

  void basicVerticalmatchStrat();

};

#endif /* AIBOARDCONTROLLER_H */

