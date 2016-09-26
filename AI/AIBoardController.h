
#ifndef AIBOARDCONTROLLER_H
#define AIBOARDCONTROLLER_H

#include<queue>
#include"../Game/BoardController.h"

class AIBoardController : public BoardController {
    
    
public:
    enum InputAction {
        UP, RIGHT, DOWN, LEFT, SWAP
    };
    
    AIBoardController(Board&);
    
    void tick() override;
    
    virtual ~AIBoardController();
private:
    std::queue<InputAction> _inputQueue;
    void doInput(InputAction);

};

#endif /* AIBOARDCONTROLLER_H */

