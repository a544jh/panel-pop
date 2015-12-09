/*
 * ScorePanel.h
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#ifndef POPUP_H_
#define POPUP_H_

#include "Renderer.h"
#include <string>

class Popup : public Renderer {
public:
	Popup();
	Popup(int x, int y, int value, int lifetime);

	int _x, _y, _value, _ticksToLive, _ticksLived;
	bool _alive;
	std::string _digits;

	virtual void tick();
	virtual void renderOnBoard() =0;
	virtual SDL_Texture* render();
	virtual ~Popup();
private:
	void initDigits();
};

#endif /* POPUP_H_ */
