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
			buttonsCount
		};
	};

	void loadXinput();
	
	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);

	float getMoveDir();

	//used internally
	namespace internal
	{
		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s);
	}

	void updateInput();

}