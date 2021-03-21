#pragma once
#include "gl2d.h"

namespace menu 
{

	void startMenu(int id);

	void uninteractableCentreText(const char *text);

	void booleanTextBox(const char *text, bool *b);

	void slider0_1(const char *text, float *val);

	void interactableText(const char *text, bool *hasPressed);

	///back pressed can be nullptr and in that case the back option will not be available
	void endMenu(gl2d::Renderer2D &renderer, gl2d::Texture backgroundTexture, gl2d::Font f, bool *backPressed, float deltaTime);

};


//todo implement get text size