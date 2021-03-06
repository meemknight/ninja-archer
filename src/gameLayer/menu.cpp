#include "menu.h"
#include <vector>
#include "Ui.h"
#include "tools.h"
#include "input.h"
#include <iomanip>
#include "gameLayer.h"
#include "platformInput.h"

struct lineInfo
{
	enum type
	{
		uninteractableCentreText,
		booleanTextBox,
		slider0_1,
		interactableText,
		textWithButton,
	};

	int type = 0;
	const char* text;
	bool *bVal;
	float *fVal;
	int buttonType = 0;
	int secondButton = -1;
};

struct
{
	int cursorIndex = 0;
	

}static perMenuData;



struct
{
	std::vector <lineInfo> lines;

}static perFrameData;

static int lastId = -1;
static int usedMouse = 0;

void menu::startMenu(int id)
{
	if(lastId != id)
	{
		perMenuData = {};
		//resetMenuState();
	}

	lastId = id;

	perFrameData.lines.clear();
}

void menu::uninteractableCentreText(const char * text)
{
	lineInfo info = {};
	info.type = info.uninteractableCentreText;
	info.text = text;
	perFrameData.lines.push_back(info);
}

void menu::booleanTextBox(const char * text, bool * b)
{
	lineInfo info = {};
	info.type = info.booleanTextBox;
	info.text = text;
	info.bVal = b;
	perFrameData.lines.push_back(info);

}

void menu::slider0_1(const char * text, float *val)
{
	lineInfo info = {};
	info.type = info.slider0_1;
	info.text = text;
	info.fVal = val;
	perFrameData.lines.push_back(info);
}

void menu::interactableText(const char * text, bool *hasPressed)
{
	lineInfo info = {};
	info.type = info.interactableText;
	info.text = text;
	info.bVal = hasPressed;
	perFrameData.lines.push_back(info);
}

void menu::textWithButton(const char* text, int buttonType, int secondButton)
{
	lineInfo info = {};
	info.type = info.textWithButton;
	info.text = text;
	info.buttonType = buttonType;
	info.secondButton = secondButton;
	perFrameData.lines.push_back(info);
}

float speed = 0.3f;


void menu::endMenu(gl2d::Renderer2D & renderer, gl2d::Texture backgroundTexture,
	gl2d::Texture backTexture, gl2d::Font f, bool * backPressed, float deltaTime)
{
	float pixelUnitX = renderer.windowW	/ 1024.f;
	float pixelUnitY = renderer.windowH / 1024.f;

	const glm::vec4 selectedButtonColor = Colors::lightGreen;
	const glm::vec4 selectedArrowColor = Colors::darkGreen;
	const glm::vec4 uninteractebleTextColor = Colors::darkGray;
	const glm::vec4 dropShadowColor = { 0.1f,0.1f,0.1f,1.f };

	//input bindings
	bool usingControllerInput = input::isControllerInput();
	bool acceptKeyReleased = input::isKeyReleased(input::Buttons::jump) || platform::isKeyReleased(platform::Button::Enter);
	bool leftPressed = input::isKeyHeld(input::Buttons::left);
	bool rightPressed = input::isKeyHeld(input::Buttons::right);
	bool upReleased = input::isKeyReleased(input::Buttons::up);
	bool downReleased = input::isKeyReleased(input::Buttons::down);
	bool escReleased = input::isKeyReleased(input::Buttons::esc);

	//


	auto c = renderer.currentCamera;
	renderer.currentCamera.setDefault();


	float inPerc = 0.9f;

	if(backgroundTexture.id)
	{
		renderer.renderRectangle({ 0,0,renderer.windowW, renderer.windowH }, {1,1,1,0.5}, {}, 0,
		backgroundTexture);
	}


#pragma region setInnerRegion
	glm::vec2 inPercXY = { renderer.windowW, renderer.windowH };
	inPercXY -= glm::vec2(renderer.windowW, renderer.windowH) * inPerc;

	Ui::Frame frame({ inPercXY.x, inPercXY.y, renderer.windowW * inPerc, renderer.windowH * inPerc });
#pragma endregion

	float posy = 20 * pixelUnitY;

	auto p = platform::getRelMousePosition();

	if(backPressed != nullptr && backTexture.id && !input::isControllerInput())
	{
		auto box = Ui::Box().xLeftPerc(0.80).yTopPerc(0).xDimensionPercentage(0.05).yAspectRatio(1) ();

		if (Ui::isInButton(p, box))
		{
			//input::drawButton(renderer, { box.x + 10, box.y }, box.z, input::Buttons::esc, 0.7f);

			if(platform::isLMouseHeld())
			{
			
				renderer.renderRectangle({ box.x, box.y + 10 * pixelUnitY, box.z, box.w }, { 1, 1, 1, 0.6 }, {}, 0, backTexture);

			}else
			{

				renderer.renderRectangle({ box.x, box.y, box.z, box.w }, { 1, 1, 1, 0.6 }, {}, 0, backTexture);

			}

			if (platform::isLMouseReleased())
			{
				*backPressed = true;
			}

		}
		else 
		{
			//input::drawButton(renderer, box, box.z, input::Buttons::esc, 1.f);
			

			renderer.renderRectangle(box, { 1,1,1,1 }, {}, 0, backTexture);

		}
		
	}

	int count = 0;
	for(auto &i: perFrameData.lines)
	{
		bool isSelected = false;
		if(perMenuData.cursorIndex == count && !usedMouse)
		{
			isSelected = true;
		}

		glm::vec4 textBox = Ui::Box().xLeft(20 * pixelUnitX).yTop((int)posy);
		glm::vec2 size = renderer.getTextSize(i.text, f, 0.7f);
		float oneLineSize = renderer.getTextSize("|", f, 0.7f).y;
		textBox.z = size.x;
		textBox.w = size.y;

		switch (i.type)
		{
		case i.uninteractableCentreText:
		{
			glm::vec4 center = Ui::Box().xCenter().yCenter()();
			renderer.renderText({ textBox.x, textBox.y }, i.text, f, uninteractebleTextColor,
				0.7f, 4, 3, false, dropShadowColor, { 0.2f,0.3f,0.5f,0.1f });
		}
			break;
		case i.booleanTextBox:
		{
			float bonusW = renderer.getTextSize(" : off ", f, 0.7f, 4, 3).x + 20 * pixelUnitX;

			glm::vec4 color = Colors::white;
			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW;

			if(!usingControllerInput)
			{
				if(Ui::isInButton(p,box) && usedMouse)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				color = selectedButtonColor;
				textBox.x += 20 * pixelUnitX;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7f, 4, 3, false);


				if (acceptKeyReleased ||
						(
						Ui::isButtonReleased(p, box)
						)
					)
				{
					if (i.bVal)
						*i.bVal = !*i.bVal;
				}
			}
			else
			{
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7f, 4, 3, false);
			}


			if (i.bVal)
			{
				const char *c;
				if (*i.bVal)
				{
					c = " : on";
				}
				else
				{
					c = " : off";
				}
				renderer.renderText({ textBox.x + textBox.z, textBox.y }, c, f, color,
					0.7f, 4, 3, false);
			}

			

			count++;
		}
			break;
		case i.slider0_1:
		{
			float bonusW = renderer.getTextSize(" : < 100%  >", f, 0.7f, 4, 3).x;
			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW + 30 * pixelUnitX;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && usedMouse)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			glm::vec4 color(1, 1, 1, 1);
			if (isSelected)
			{
				color = selectedButtonColor;
				textBox.x += 20 * pixelUnitX;
			}


			float bonusLeft1 = renderer.getTextSize(" :", f, 0.7f, 4, 3).x;
			float size = renderer.getTextSize(" < ", f, 0.7f, 4, 3).x;
			bool leftArrowIn = Ui::isInButton(p,
				{ textBox.x + textBox.z + bonusLeft1, box.y, size, box.w });
			bool rightArrowIn = Ui::isInButton(p,
				{ textBox.x + textBox.z + bonusW - size, box.y, size, box.w });


			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && usedMouse)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if(isSelected)
			{
				if (i.fVal)
				{

					if (leftPressed || (leftArrowIn && platform::isLMouseHeld()))
					{
						leftArrowIn = true;
						*i.fVal -= deltaTime * speed;
					}
					if (rightPressed || (rightArrowIn && platform::isLMouseHeld()))
					{
						rightArrowIn = true;
						*i.fVal += deltaTime * speed;
					}

					if (*i.fVal < 0) { *i.fVal = 0; }
					if (*i.fVal > 1) { *i.fVal = 1; }
				}

			}

			renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7f, 4, 3, false, dropShadowColor);

			float v = *i.fVal * 100;

			//int precision = 1;
			//if (v == 100)
			//{
			//	precision = 0;
			//}else if ( v < 10)
			//{
			//	precision = 2;
			//}

			std::stringstream stream;
			//stream << std::fixed << std::setprecision(precision) << v;
			stream << (int)v;
			std::string s = stream.str();

			float bonus = 0;
			//renderer.renderText({ textBox.x + textBox.z, textBox.y }, (" : < " + stream.str() + "% >").c_str(), f, color,
			//	0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			{
				auto leftColor = color;
				auto rightColor = color;

				if(leftArrowIn)
				{
					leftColor = selectedArrowColor;
				}

				if (rightArrowIn)
				{
					rightColor = selectedArrowColor;
				}

				renderer.renderText({ textBox.x + textBox.z, textBox.y }, " : ", f, color,
					0.7f, 4, 3, false, dropShadowColor);
				bonus += renderer.getTextSize("  :", f, 0.7f, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, "< ", f, leftColor,
					0.7f, 4, 3, false, dropShadowColor);
				bonus += renderer.getTextSize(" <", f, 0.7f, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, (stream.str() + "% ").c_str(), f, color,
					0.7f, 4, 3, false, dropShadowColor);
				bonus += renderer.getTextSize((stream.str() + " %").c_str(), f, 0.7f, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, ">", f, rightColor,
					0.7f, 4, 3, false, dropShadowColor);
			}

			count++;
		}
			break;
		case i.interactableText:
		{
			float bonusW = renderer.getTextSize(" ", f, 0.7f, 4, 3).x + 20 * pixelUnitX;

			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && usedMouse)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				textBox.x += 20 * pixelUnitX;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, selectedButtonColor,
					0.7f, 4, 3, false, dropShadowColor);

				if (acceptKeyReleased ||
					(
					Ui::isButtonReleased(p, box)
					)
					)
				{
					if (i.bVal)
						*i.bVal = true;
				}
			}
			else
			{
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, Colors::white,
					0.7f, 4, 3, false, dropShadowColor);
			}

			count++;
		}
			break;
		case i.textWithButton:
		{
			float buttonSize = renderer.getTextSize("X", f, 0.7).x * 1.5;

			glm::vec4 center = Ui::Box().xCenter().yCenter()();
			renderer.renderText({ textBox.x, textBox.y }, i.text, f, uninteractebleTextColor,
				0.7f, 4, 3, false, dropShadowColor, { 0.2f,0.3f,0.5f,0.1f });

			auto size = renderer.getTextSize(i.text, f, 0.7);

			input::drawButton(renderer, { textBox.x + size.x + size.y/2, textBox.y - size.y/1.5f }, buttonSize, i.buttonType);

			if (i.secondButton > -1)
			{
				input::drawButton(renderer, { textBox.x + size.x + size.y * 2, textBox.y - size.y / 1.5f },
					buttonSize, i.secondButton);

			}
		
		}
			break;

		default:
			break;
		}

		posy += 20 * pixelUnitY + textBox.w;

	}

	if (!usedMouse)
	{
		if (upReleased)
		{
			perMenuData.cursorIndex--;
		}

		if (downReleased)
		{
			perMenuData.cursorIndex++;
		}
	}
	
	if (escReleased)
	{
		if(backPressed)
		{
			*backPressed = true;
		}
	}


	if(perMenuData.cursorIndex < 0)
	{
		perMenuData.cursorIndex = count - 1;
	}
	else
	if(perMenuData.cursorIndex >= count)
	{
		perMenuData.cursorIndex = 0;
	}

	if (upReleased || downReleased || leftPressed || rightPressed)
	{
		usedMouse = false;
	}

	if (platform::mouseMoved())
	{
		usedMouse = true;
		perMenuData.cursorIndex = 0;
	}

	renderer.currentCamera = c;

}
