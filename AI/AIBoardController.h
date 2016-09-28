
#ifndef AIBOARDCONTROLLER_H
#define AIBOARDCONTROLLER_H

#include<queue>
#include"../Game/BoardController.h"

class AIBoardController : public BoardController {
    
    
public:
    enum InputAction {
        UP, RIGHT, DOWN, LEFT, SWAP, WAIT
    };
    
    AIBoardController(Board&);
    
    void tick() override;
    
    virtual ~AIBoardController();
private:
    std::queue<InputAction> _inputQueue;
    void doInput(InputAction);
    
    void moveBlock(int x, int y, int dx, int dy);
    void moveCursorTo(int x, int y);
    
};

#endif /* AIBOARDCONTROLLER_H */

