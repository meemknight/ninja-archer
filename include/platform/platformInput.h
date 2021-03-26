#pragma once
#include "gameLayer.h"
#include <GLFW/glfw3.h>

namespace platform 
{
	struct Button
	{
		bool pressed = 0;
		bool held = 0;
		bool released = 0;
		char newState = -1; // this can be -1

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
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E, GLFW_KEY_F, GLFW_KEY_G,
			GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N,
			GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T, GLFW_KEY_U, 
			GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y, GLFW_KEY_Z,
			GLFW_KEY_0, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6,
			GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9,
			GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT
		};

		void merge(const Button &b)
		{
			this->pressed |= b.pressed;
			this->released |= b.released;
			this->held |= b.held;
		}
	};

	namespace internal
	{
		inline void resetButtonToZero(Button &b);
	}

	struct ControllerButtons
	{
		enum Buttons
		{
		A = GLFW_GAMEPAD_BUTTON_A,           
		B = GLFW_GAMEPAD_BUTTON_B,           
		X = GLFW_GAMEPAD_BUTTON_X,           
		Y = GLFW_GAMEPAD_BUTTON_Y,           
		LBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, 
		RBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		Back = GLFW_GAMEPAD_BUTTON_BACK,
		Start = GLFW_GAMEPAD_BUTTON_START,       
		Guide = GLFW_GAMEPAD_BUTTON_GUIDE,      
		LThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,  
		Rthumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, 
		Up = GLFW_GAMEPAD_BUTTON_DPAD_UP,   
		Right = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,  
		Down = GLFW_GAMEPAD_BUTTON_DPAD_DOWN, 
		Left = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,  
		ButtonCount = 15,
		};

		Button buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];

		float LT;
		float RT;

		struct
		{
			float x, y;
		}LStick, RStick;

		void setAllToZero()
		{
			
			for(int i=0; i< GLFW_GAMEPAD_BUTTON_LAST+1; i++)
			{
				internal::resetButtonToZero(buttons[i]);
			}

			LT = 0;
			RT = 0;

			LStick = {};
			RStick = {};

		}
	};

	
	//Button::key
	int isKeyHeld(int key);

	int isKeyPressedOn(int key);

	int isKeyReleased(int key);

	int isLMousePressed();
	int isRMousePressed();

	int isLMouseReleased();
	int isRMouseReleased();

	int isLMouseHeld();
	int isRMouseHeld();

	bool keyboardMousePressed();

	ControllerButtons getControllerButtons();

	namespace internal
	{

		void setButtonState(int button, int newState);

		void setLeftMouseState(int newState);
		void setRightMouseState(int newState);

		inline void processEventButton(Button &b, bool newState)
		{
			b.newState = newState;
		}

		inline void updateButton(Button &b)
		{
			if (b.newState == 1)
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
			else if(b.newState == 0)
			{
				b.held = false;
				b.pressed = false;
				b.released = true;
			}else
			{
				b.pressed = false;
				b.released = false;
			}

			b.newState = -1;
		}

		inline void updateButtonInplace(Button &b, bool state)
		{
			if (state == 1)
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
			else if(b.held)
			{
				b.held = false;
				b.pressed = false;
				b.released = true;
			}else
			{
				b.held = false;
				b.pressed = false;
				b.released = false;
			}
			

			b.newState = -1;
		}

		void updateAllButtons();
		void resetInputsToZero();
	};

};