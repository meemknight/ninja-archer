#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <glm/vec2.hpp>

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
			buttonsCount
		};
	};

	void loadXinput();
	
	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);
	bool isKeyReleased(int b);

	int getMoveDir();

	glm::vec2 getShootDir(glm::vec2 centre);

	//used internally
	namespace internal
	{
		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s);
	}

	void updateInput();

}