/*
 * ScorePanel.h
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#ifndef SCOREPANEL_H_
#define SCOREPANEL_H_

#include "Renderer.h"

class ScorePanel : public Renderer {
public:
	ScorePanel();

	int _x, _y, _h, _w, _value, _ticksAlive;
	bool _alive;

	virtual void tick();

	virtual ~ScorePanel();
private:
	//virtual SDL_Texture* createTexture();
};

#endif /* SCOREPANEL_H_ */
