/*
 * TitleScreenRenderer.h
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#ifndef RENDERING_TITLESCREENRENDERER_H_
#define RENDERING_TITLESCREENRENDERER_H_

#include "Renderer.h"

class TitleScreenRenderer : public Renderer {
public:
	TitleScreenRenderer();
	virtual ~TitleScreenRenderer();
	void tick();
	SDL_Texture* render();
private:
	SDL_Texture* _titleImg;
	SDL_Texture* _versionText;
};

#endif /* RENDERING_TITLESCREENRENDERER_H_ */
