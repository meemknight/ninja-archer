#include "input.h"
#include <Windows.h>
#include"gameLayer.h"
#include <math.h>
#include <algorithm>
#include "glm/glm.hpp"
#include "tools.h"
#include <cmath>
#include <ctime>

extern gl2d::Texture uiButtons;
extern gl2d::Font font;

namespace input
{

	typedef DWORD WINAPI XInputGetState_t(DWORD dwUserIndex, XINPUT_STATE* pState);
	static XInputGetState_t *DynamicXinputGetState;
	typedef DWORD WINAPI XInputSetState_t(DWORD dwUserIndex, XINPUT_VIBRATION* pState);
	static XInputSetState_t *DynamicXinputSetState;
	typedef DWORD WINAPI XInputGetKeystroke_t(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
	static XInputGetKeystroke_t *DynamicXInputGetKeystroke;

	static bool xInputLoaded = 0;
	static bool usedController = 0;

	void loadXinput()
	{
		//todo also load 9-0-1
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

	int bindings[Buttons::buttonsCount] = { 0, 'S', 'A', 'D', VK_SPACE, 0, 'W', 'Q', 'E', VK_ESCAPE, VK_ESCAPE };
	WORD bindingsController[Buttons::buttonsCount] = { 0, XINPUT_GAMEPAD_DPAD_DOWN
		, XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT, XINPUT_GAMEPAD_A, 0, XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_START };
	float deadZone = 0.28f;
	float moveSensitivity = 0.25f;

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

	bool isControllerInput()
	{
		return usedController;
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
		bool read = 0;
		if (xInputLoaded != 0)
		{
			for(int i=0; i<3; i++)
			{
				if (DynamicXinputGetState(i, &s) != ERROR_SUCCESS)
				{
					read = 1;
					break;
				}
			}

		}

#pragma region determin whether controller or not
		if(read)
		{
			

			XINPUT_KEYSTROKE ks = {};
			if(DynamicXInputGetKeystroke && DynamicXInputGetKeystroke(0, 0, &ks) == ERROR_SUCCESS)
			{
				if(ks.Flags & (XINPUT_KEYSTROKE_REPEAT | XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_KEYUP))
				{
					usedController = true;
				}
			}

			for(int i='0'; i<='z';i++)
			{
				if(GetAsyncKeyState(i))
				{
					usedController = false;
					break;
				}
			}
			if(GetAsyncKeyState(VK_SPACE) ||
				GetAsyncKeyState(VK_BACK) ||
				GetAsyncKeyState(VK_CONTROL) ||
				GetAsyncKeyState(VK_DOWN) ||
				GetAsyncKeyState(VK_UP) ||
				GetAsyncKeyState(VK_LEFT) ||
				GetAsyncKeyState(VK_RIGHT) ||
				GetAsyncKeyState(VK_LSHIFT) ||
				GetAsyncKeyState(VK_SHIFT)||
				GetAsyncKeyState(VK_ACCEPT)
				)
			{
				usedController = false;
			}

			if(platform::isLMouseHeld() 
				|| platform::isRMouseHeld()
				||platform::mouseMoved()
				)
			{
				usedController = false;
			}

		}else
		{
			usedController = false;
		}
#pragma endregion

		if (read)
		{
			//todo proper normalize
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
			if (read)
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

	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a)
	{
		drawButton(renderer, pos, size, button, input::isControllerInput(),a);
	}

	void drawButtonWithHover(gl2d::Renderer2D & renderer, glm::vec2 pos, float size, int button, float a)
	{
		float f = (std::sin(clock() / 220.f) / 2.f + 1) * 3.5;
		pos.y -= f;
		drawButton(renderer, pos, size, button, a);
	}

	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, bool isController, float a)
	{
		glm::ivec2 s = uiButtons.GetSize();
		gl2d::TextureAtlasPadding uiAtlas(Buttons::buttonsCount, 2, s.x, s.y);

		renderer.renderRectangle({ pos.x, pos.y, size, size }, {0,0,0,a}, {}, 0, uiButtons, uiAtlas.get(button, isController));
		
		pos.y -= 1;
		pos.x += 1;
		
		renderer.renderRectangle({ pos.x, pos.y, size, size }, {1,1,1,a}, {}, 0, uiButtons, uiAtlas.get(button, isController));

	}

	namespace internal
	{

		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s)
		{
			if(!platform::isFocused())
			{
				return 0;
			}

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

					val = retValY < (-moveSensitivity * 3.8);
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

					val = retValY > moveSensitivity * 3.8;
					val = val | (pad->wButtons & bindingsController[b]);
				}
				else
				{
					val = (pad->wButtons & bindingsController[b]);
				}

			}
			
			if (b == input::Buttons::shoot)
			{
				val = val || platform::isLMouseHeld();
			}
			else
			{
				val = val || platform::isKeyHeld(bindings[b]);
			}

			return val;

		}
	}


};
