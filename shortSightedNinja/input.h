#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <glm/vec2.hpp>
#include "opengl2Dlib.h"
#include <string>

namespace input
{


	struct Button
	{
		char pressed = 0;
		char held = 0;
		char released = 0;

		enum
		{
			A = 0,
			B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			NR0, NR1, NR2, NR3, NR4, NR5, NR6, NR7, NR8, NR9,
			Space,
			Enter,
			Escape,
			Up,
			Down,
			Left,
			Right,
			BUTTONS_COUNT, //
		};

		static constexpr int buttonValues[BUTTONS_COUNT] =
		{
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
			'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			VK_SPACE, VK_RETURN, VK_ESCAPE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT
		};

		void merge(const Button &b)
		{
			this->pressed |= b.pressed;
			this->released |= b.released;
			this->held |= b.held;
		}
	};

	inline void processEventButton(Button &b, bool newState)
	{

		if (newState)
		{
			if (b.held)
			{
				b.pressed = false;
			}
			else
			{
				b.pressed = true;
			}

			b.held = true;
			b.released = false;
		}
		else
		{
			b.held = false;
			b.pressed = false;
			b.released = true;
		}


	}

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

	void loadXinput();
	
	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);
	bool isKeyReleased(int b);


	int getMoveDir();

	bool isControllerInput();

	glm::vec2 getShootDir(glm::vec2 centre);

	//used internally
	namespace internal
	{
		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s);
	}

	void updateInput();

	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a= 1.f);
	void drawButtonWithHover(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a = 1.f);
	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size ,int button, bool isController, float a = 1.f);
	void drawButton(gl2d::Renderer2D& renderer, glm::vec2 pos, float size, int button, bool isController, float a, glm::vec2 shadow, float shadowA);



}