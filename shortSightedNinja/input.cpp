#include "input.h"
#include <Windows.h>
#include"gameLayer.h"
#include <math.h>
#include <algorithm>
#include "glm/glm.hpp"

namespace input
{

	typedef DWORD WINAPI XInputGetState_t(DWORD dwUserIndex, XINPUT_STATE* pState);
	static XInputGetState_t *DynamicXinputGetState;
	typedef DWORD WINAPI XInputSetState_t(DWORD dwUserIndex, XINPUT_VIBRATION* pState);
	static XInputSetState_t *DynamicXinputSetState;
	typedef DWORD WINAPI XInputGetKeystroke_t(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
	XInputGetKeystroke_t *DynamicXInputGetKeystroke;

	static bool xInputLoaded = 0;

	void loadXinput()
	{

		HMODULE xinputLib = LoadLibrary("xinput1_4.dll");
		if (!xinputLib)
		{
			xinputLib = LoadLibrary("xinput1_3.dll");
		}
		if (!xinputLib)
		{
			return;
		}

		{
			DynamicXinputGetState = (XInputGetState_t*)GetProcAddress(xinputLib, "XInputGetState");
			DynamicXinputSetState = (XInputSetState_t*)GetProcAddress(xinputLib, "XInputSetState");
			DynamicXInputGetKeystroke = (XInputGetKeystroke_t*)GetProcAddress(xinputLib, "XInputGetKeystroke");
			xInputLoaded = 1;
		}

	}

	int bindings[Buttons::buttonsCount] = { 0, 'S', 'A', 'D', VK_SPACE, 0, 'W', 'Q', 'E' };
	WORD bindingsController[Buttons::buttonsCount] = { 0, XINPUT_GAMEPAD_DPAD_DOWN
		, XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT, XINPUT_GAMEPAD_A, 0, XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER };
	float deadZone = 0.12f;
	float moveSensitivity = 0.20f;

	int buttonsHeld[Buttons::buttonsCount] = {};
	int buttonsPressed[Buttons::buttonsCount] = {};
	int buttonsReleased[Buttons::buttonsCount] = {};

	bool isKeyPressedOn(int b)
	{
		return buttonsPressed[b];
	}
	
	bool isKeyHeld(int b) 
	{
		return buttonsHeld[b];
	}

	bool isKeyReleased(int b)
	{
		return buttonsReleased[b];
	}

	int getMoveDir()
	{
		return -isKeyHeld(Buttons::left) + isKeyHeld(Buttons::right);
	}

	glm::vec2 lastShootDir = {1,0};

	glm::vec2 getShootDir(glm::vec2 centre)
	{

		if(platform::mouseMoved())
		{
			lastShootDir = glm::vec2(platform::getRelMousePosition()) - centre;
			lastShootDir = glm::normalize(lastShootDir);
		}

		return lastShootDir;

	}

	void updateInput()
	{
		XINPUT_STATE s;
		bool read = 1;
		if (xInputLoaded == 0 || DynamicXinputGetState(0, &s) != ERROR_SUCCESS)
		{
			read = 0;
		}

		if (read)
		{
			const XINPUT_GAMEPAD *pad = &s.Gamepad;
			float retValX = pad->sThumbRX / (float)SHRT_MAX;
			float retValY = -pad->sThumbRY / (float)SHRT_MAX;

			retValX = std::max(-1.f, retValX);
			retValX = std::min(1.f, retValX);

			retValY = std::max(-1.f, retValY);
			retValY = std::min(1.f, retValY);

			if (abs(retValX) < deadZone && abs(retValY) < deadZone)
			{

			}
			else
			{
				lastShootDir = { retValX, retValY };
				lastShootDir = glm::normalize(lastShootDir);
			}
			
		
		}


		for (int i = 0; i < Buttons::buttonsCount; i++)
		{
			bool state;
			if (read == 1)
			{
				state = internal::getisKeyHeldDirect(i, &s);
			}
			else
			{
				state = internal::getisKeyHeldDirect(i, nullptr);
			}

			if(!state && buttonsHeld[i])
			{
				buttonsReleased[i] = 1;
			}else
			{
				buttonsReleased[i] = 0;
			}

			buttonsPressed[i] = 0;

			if (state)
			{
				if (buttonsHeld[i] == 0)
				{
					buttonsPressed[i] = 1;
				}
			}
			buttonsHeld[i] = state;

		}

	}

	namespace internal
	{

		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s)
		{

			bool val = 0;

			if (s)
			{
				const XINPUT_GAMEPAD *pad = &s->Gamepad;

				
				if (b == input::Buttons::jump)
				{
					val = pad->bLeftTrigger > 20;
					val = val | (pad->wButtons & bindingsController[b]);
				}
				else if (b == input::Buttons::shoot)
				{
					val = pad->bRightTrigger > 20;
				}
				else if(b == input::Buttons::left)
				{
					float retValX = pad->sThumbLX / (float)SHRT_MAX;

					retValX = std::max(-1.f, retValX);
					retValX = std::min(1.f, retValX);

					if (abs(retValX) < deadZone)
					{
						retValX = 0.f;
					}

					val = retValX < -moveSensitivity;
					val =  val | (pad->wButtons & bindingsController[b]);
				}else if(b == input::Buttons::right)
				{
					float retValX = pad->sThumbLX / (float)SHRT_MAX;

					retValX = std::max(-1.f, retValX);
					retValX = std::min(1.f, retValX);

					if (abs(retValX) < deadZone)
					{
						retValX = 0.f;
					}

					val = retValX > moveSensitivity;
					val = val | (pad->wButtons & bindingsController[b]);
				}else if(b == input::Buttons::down)
				{
					float retValY = pad->sThumbLY / (float)SHRT_MAX;

					retValY = std::max(-1.f, retValY);
					retValY = std::min(1.f, retValY);

					if (abs(retValY) < deadZone)
					{
						retValY = 0.f;
					}

					val = retValY < (-moveSensitivity * 2);
					val = val | (pad->wButtons & bindingsController[b]);
				}else if (b == input::Buttons::up)
				{
					float retValY = pad->sThumbLY / (float)SHRT_MAX;

					retValY = std::max(-1.f, retValY);
					retValY = std::min(1.f, retValY);

					if (abs(retValY) < deadZone)
					{
						retValY = 0.f;
					}

					val = retValY > moveSensitivity;
					val = val | (pad->wButtons & bindingsController[b]);
				}
				else
				{
					val = (pad->wButtons & bindingsController[b]);
				}

			}
			
			if (b == input::Buttons::shoot)
			{
				val = val || platform::isLMouseButtonPressed();
			}
			else
			{
				val = val || platform::isKeyHeld(bindings[b]);
			}

			return val;

		}
	}


};
