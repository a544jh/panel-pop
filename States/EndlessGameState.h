/*
 * EndlessGameState.h
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */


#ifndef STATES_ENDLESSGAMESTATE_H_
#define STATES_ENDLESSGAMESTATE_H_

#include "State.h"
#include "GameState.h"

#include "../Game/EndlessGame.h"
#include "../Game/KeyboardController.h"
#include "../Rendering/EndlessGameRenderer.h"


class EndlessGameState : public GameState {
public:
    EndlessGameState();
    virtual ~EndlessGameState();

private:

};

#endif /* STATES_ENDLESSGAMESTATE_H_ */
