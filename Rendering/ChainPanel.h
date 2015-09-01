/*
 * ChainPanel.h
 *
 *  Created on: Aug 25, 2015
 *      Author: axel
 */

#ifndef CHAINPANEL_H_
#define CHAINPANEL_H_

#include "ScorePanel.h"

class ChainPanel : public ScorePanel {
public:
	ChainPanel();
	ChainPanel(int x, int y, int value);
	virtual ~ChainPanel();
private:
	SDL_Texture* createTexture();
};

#endif /* CHAINPANEL_H_ */
