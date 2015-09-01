/*
 * ScorePanel.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#include "ScorePanel.h"

ScorePanel::ScorePanel() : _alive(true){
}

void ScorePanel::tick() {
	--_y;
	if (--_ticksAlive <= 0){
		_alive = false;
	}
}

ScorePanel::~ScorePanel() {
	// TODO Auto-generated destructor stub
}

