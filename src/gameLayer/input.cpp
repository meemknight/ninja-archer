#include "input.h"
//#include <Windows.h>
#include"gameLayer.h"
#include <math.h>
#include <algorithm>
#include "glm/glm.hpp"
#include "tools.h"
#include <cmath>
#include <ctime>
#include "platformInput.h"

extern gl2d::Texture uiButtons;
extern gl2d::Font font;

namespace input
{


	int buttonsHeld[Buttons::buttonsCount] = {};
	int buttonsPressed[Buttons::buttonsCount] = {};
	int buttonsReleased[Buttons::buttonsCount] = {};

	bool isKeyPressedOn(int b)
	{

		return buttonsPressed[b] && platform::isFocused();
	}

	bool isKeyHeld(int b)
	{
		return buttonsHeld[b] && platform::isFocused();
	}

	bool isKeyReleased(int b)
	{
		return buttonsReleased[b] && platform::isFocused();
	}


	int getMoveDir()
	{
		return -(int)input::isKeyHeld(input::Buttons::left) + (int)input::isKeyHeld(input::Buttons::right);
	}

	bool isControllerInputFlag = false;
	bool isControllerInput()
	{
		return isControllerInputFlag;
	}

	glm::vec2 lastShootDir = { 1,0 };

	glm::vec2 getShootDir(glm::vec2 centre)
	{
		float x = platform::getControllerButtons().RStick.x;
		float y = platform::getControllerButtons().RStick.y;

		glm::vec2 dir(x, y);

		if(glm::length(dir) > 0.5)
		{
			dir /= glm::length(dir);
			lastShootDir = dir;
			return dir;
		}

		if (platform::mouseMoved())
		{
			lastShootDir = glm::vec2(platform::getRelMousePosition()) - centre;
			lastShootDir = glm::normalize(lastShootDir);
		}

		return lastShootDir;

	}


platform::Button jumpButton;
platform::Button shootButton;
platform::Button upButton;
platform::Button downButton;
platform::Button menuButton;
platform::Button backButton;

int buttonMapping1[Buttons::buttonsCount] =
{
	0,
	platform::Button::S, 
	platform::Button::A,
	platform::Button::D,
	platform::Button::Space,
	platform::Button::Enter, //shoot
	platform::Button::W,
	platform::Button::Q,
	platform::Button::E,
	platform::Button::Escape,
	platform::Button::Escape,
};

	void updateInput()
	{
		{
			if(platform::keyboardMousePressed() || platform::mouseMoved())
			{
				isControllerInputFlag = 0;
			}

			for (int i = 0; i < platform::ControllerButtons::Buttons::ButtonCount; i++)
			{
				if (platform::getControllerButtons().buttons[i].held)
				{
					isControllerInputFlag = 1;
					break;
				}
			}

			if(
				platform::getControllerButtons().LT > 0.3 ||
				platform::getControllerButtons().RT > 0.3 ||
				platform::getControllerButtons().LStick.x > 0.3 ||
				platform::getControllerButtons().LStick.x < -0.3 ||
				platform::getControllerButtons().LStick.y > 0.3 ||
				platform::getControllerButtons().LStick.y < -0.3 ||
				platform::getControllerButtons().RStick.x > 0.3 ||
				platform::getControllerButtons().RStick.x < -0.3 ||
				platform::getControllerButtons().RStick.y > 0.3 ||
				platform::getControllerButtons().RStick.y < -0.3 
				)
			{
				isControllerInputFlag = 1;
			}

		}

		for (int i = 0; i < Buttons::buttonsCount; i++)
		{
			platform::Button b = {};
			if(i == Buttons::none)
			{
				
			}
			else
			{
				if (i == Buttons::shoot)
				{
					bool state = 0;

					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::B];
					auto controllerC = platform::getControllerButtons().LT;

					state |= (bool)controllerB.pressed;
					state |= (bool)controllerB.held;

					if (controllerC > 0.3)
					{
						state = 1;
					}

					state |= (bool)platform::isKeyPressedOn(buttonMapping1[i]);
					state |= (bool)platform::isKeyHeld(buttonMapping1[i]);

					state |= (bool)platform::isKeyPressedOn(platform::Button::Enter);
					state |= (bool)platform::isKeyHeld(platform::Button::Enter);

					state |= platform::isLMousePressed();
					state |= platform::isLMouseHeld();

					platform::internal::updateButtonInplace(shootButton, state);

					b = shootButton;

				}else if (i == Buttons::jump)
				{
					bool state = 0;

					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::A];
					auto controllerC = platform::getControllerButtons().RT;

					state |= (bool)controllerB.pressed;
					state |= (bool)controllerB.held;

					if (controllerC > 0.3)
					{
						state = 1;
					}

					state |= (bool)platform::isKeyPressedOn(buttonMapping1[i]);
					state |= (bool)platform::isKeyHeld(buttonMapping1[i]);

					platform::internal::updateButtonInplace(jumpButton, state);

					b = jumpButton;
				}else if (i == Buttons::up)
				{

					bool state = 0;

					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::Up];
					auto controllerC = platform::getControllerButtons().LStick.y;

					state |= (bool)controllerB.pressed;
					state |= (bool)controllerB.held;

					if (controllerC < -0.7)
					{
						state = 1;
					}

					state |= (bool)platform::isKeyPressedOn(platform::Button::Up);
					state |= (bool)platform::isKeyHeld(platform::Button::Up);

					platform::internal::updateButtonInplace(upButton, state);

					b = upButton;
				}
				else if (i == Buttons::down)
				{

					bool state = 0;

					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::Down];
					auto controllerC = platform::getControllerButtons().LStick.y;

					state |= (bool)controllerB.pressed;
					state |= (bool)controllerB.held;

					if (controllerC > 0.7)
					{
						state = 1;
					}

					state |= (bool)platform::isKeyPressedOn(platform::Button::Down);
					state |= (bool)platform::isKeyHeld(platform::Button::Down);

					platform::internal::updateButtonInplace(downButton, state);

					b = downButton;
				}else if(i == Buttons::menu)
				{
					bool state = 0;

					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::Start];

					state |= (bool)controllerB.pressed;
					state |= (bool)controllerB.held;

					state |= (bool)platform::isKeyPressedOn(buttonMapping1[i]);
					state |= (bool)platform::isKeyHeld(buttonMapping1[i]);

					platform::internal::updateButtonInplace(menuButton, state);

					b = menuButton;
				}
				else if (i == Buttons::esc)
				{
				bool state = 0;

				auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::B];

				state |= (bool)controllerB.pressed;
				state |= (bool)controllerB.held;

				state |= (bool)platform::isKeyPressedOn(buttonMapping1[i]);
				state |= (bool)platform::isKeyHeld(buttonMapping1[i]);

				platform::internal::updateButtonInplace(backButton, state);

				b = backButton;
				}
				if(i == Buttons::left)
				{
					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::Left];
					auto controllerC = platform::getControllerButtons().LStick.x;

					if (controllerC < -0.4)
					{
						b.pressed = 1;
						b.held = 1;
						b.released = 1;
					}

					b.pressed |= (bool)platform::isKeyPressedOn(platform::Button::Left);
					b.held |= (bool)platform::isKeyHeld(platform::Button::Left);
					b.released |= (bool)platform::isKeyReleased(platform::Button::Left);

					b.pressed |= controllerB.pressed;
					b.held |= controllerB.held;
					b.released |= controllerB.released;
				}

				if (i == Buttons::right)
				{
					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::Right];
					auto controllerC = platform::getControllerButtons().LStick.x;

					if (controllerC > 0.4)
					{
						b.pressed = 1;
						b.held = 1;
						b.released = 1;
					}

					b.pressed |= (bool)platform::isKeyPressedOn(platform::Button::Right);
					b.held |= (bool)platform::isKeyHeld(platform::Button::Right);
					b.released |= (bool)platform::isKeyReleased(platform::Button::Right);

					b.pressed |= controllerB.pressed;
					b.held |= controllerB.held;
					b.released |= controllerB.released;
				}

				if (i == Buttons::swapLeft)
				{
					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::LBumper];
					
					b.pressed |= (bool)controllerB.pressed;
					b.held |= (bool)controllerB.held;
					b.released |= (bool)controllerB.released;
				}

				if (i == Buttons::swapRight)
				{
					auto controllerB = platform::getControllerButtons().buttons[platform::ControllerButtons::RBumper];

					b.pressed |= (bool)controllerB.pressed;
					b.held |= (bool)controllerB.held;
					b.released |= (bool)controllerB.released;
				}

				
				b.pressed |= (bool)platform::isKeyPressedOn(buttonMapping1[i]);
				b.held |= (bool)platform::isKeyHeld(buttonMapping1[i]);
				b.released |= (bool)platform::isKeyReleased(buttonMapping1[i]);
			}

			buttonsReleased[i] = b.released;

			if(buttonsHeld[i])
			{
				buttonsPressed[i] = 0;
			}else
			{
				buttonsPressed[i] = b.pressed;
			}
			buttonsHeld[i] = b.held;
	
		}
	
	}



	void drawButton(gl2d::Renderer2D &renderer, glm::vec2 pos, float size, int button, float a)
	{
		drawButton(renderer, pos, size, button, input::isControllerInput(),a);
	}

	void drawButtonWithHover(gl2d::Renderer2D & renderer, glm::vec2 pos, float size, int button, float a)
	{
		float f = (std::sin(clock() / 220.f) / 2.f + 1) * 3.5f;
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

	void drawButton(gl2d::Renderer2D& renderer, glm::vec2 pos, float size, int button, bool isController, float a, glm::vec2 shadow, float shadowA)
	{
		glm::ivec2 s = uiButtons.GetSize();
		gl2d::TextureAtlasPadding uiAtlas(Buttons::buttonsCount, 2, s.x, s.y);

		renderer.renderRectangle({ pos.x, pos.y, size, size }, { 0,0,0,shadowA }, {}, 0, uiButtons, uiAtlas.get(button, isController));

		pos.y -= shadow.y;
		pos.x += shadow.x;

		renderer.renderRectangle({ pos.x, pos.y, size, size }, { 1,1,1,a }, {}, 0, uiButtons, uiAtlas.get(button, isController));

	}

	/*
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
					val = val || (pad->wButtons & bindingsController[b]);
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
					val =  val || (pad->wButtons & bindingsController[b]);
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
					val = val || (pad->wButtons & bindingsController[b]);
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
					val = val || (pad->wButtons & bindingsController[b]);
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
					val = val || (pad->wButtons & bindingsController[b]);
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
			else if(b == input::Buttons::left)
			{
				val = val || platform::isKeyHeld(bindings[b]) || platform::isKeyHeld(Button::Left);
			}
			else if (b == input::Buttons::right)
			{
				val = val || platform::isKeyHeld(bindings[b]) || platform::isKeyHeld(Button::Right);
			}
			else if (b == input::Buttons::down)
			{
				val = val || platform::isKeyHeld(bindings[b]) || platform::isKeyHeld(Button::Down);
			}
			else if (b == input::Buttons::up)
			{
				val = val || platform::isKeyHeld(bindings[b]) || platform::isKeyHeld(Button::Up);
			}
			else if (b == input::Buttons::jump)
			{
				val = val || platform::isKeyHeld(bindings[b]) || platform::isKeyHeld(Button::Enter);
			}
			else
			{
				val = val || platform::isKeyHeld(bindings[b]);
			}

			return val;

		}
	}
	*/

};
