#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <glm/vec2.hpp>
#include "gl2d.h"
#include <string>

namespace input
{

	namespace Buttons
	{
		enum
		{
			none = 0,
			down,
			left,
			right,
			jump,
			shoot,
			up,
			swapLeft,
			swapRight,
			esc,
			menu,
			buttonsCount
		};
	};

	

	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);
	bool isKeyReleased(int b);


	int getMoveDir();

	inline bool isControllerInput(); //todo implement 

	glm::vec2 getShootDir(glm::vec2 centre);

	
	void updateInput();

	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a= 1.f);
	void drawButtonWithHover(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a = 1.f);
	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size ,int button, bool isController, float a = 1.f);
	void drawButton(gl2d::Renderer2D& renderer, glm::vec2 pos, float size, int button, bool isController, float a, glm::vec2 shadow, float shadowA);



}