#include "menu.h"
#include <vector>
#include "Ui.h"
#include "tools.h"
#include "input.h"
#include <iomanip>
#include "gameLayer.h"

struct lineInfo
{
	enum type
	{
		uninteractableCentreText,
		booleanTextBox,
		slider0_1,
		interactableText,
	};

	int type = 0;
	const char* text;
	bool *bVal;
	float *fVal;
};

struct
{
	int cursorIndex = 0;
	int lastcursorIndex = 0;

}perMenuData;

struct
{
	std::vector <lineInfo> lines;

}perFrameData;


void menu::startMenu()
{
	perFrameData = {};
}

void menu::resetMenuState()
{
	perMenuData = {};
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

float speed = 0.3;

const glm::vec4 selectedButtonColor = Colors::lightGreen;
const glm::vec4 selectedArrowColor = Colors::darkGreen;
const glm::vec4 uninteractebleTextColor = Colors::darkGray;

void menu::endMenu(gl2d::Renderer2D & renderer, gl2d::Texture backgroundTexture, gl2d::Font f, bool * backPressed, float deltaTime)
{
	//input bindings
	///todo add your own functions here

	bool usingControllerInput = input::isControllerInput();
	bool acceptKeyReleased = input::isKeyReleased(input::Buttons::jump);
	bool leftPressed = input::isKeyHeld(input::Buttons::left);
	bool rightPressed = input::isKeyHeld(input::Buttons::right);
	bool upReleased = input::isKeyReleased(input::Buttons::up);
	bool downReleased = input::isKeyReleased(input::Buttons::down);
	bool escReleased = input::isKeyReleased(input::Buttons::esc);

	//


	auto c = renderer.currentCamera;
	renderer.currentCamera.setDefault();


	float inPerc = 0.9;

	renderer.renderRectangle({ 0,0,renderer.windowW, renderer.windowH }, {}, 0,
		backgroundTexture);

#pragma region setInnerRegion
	glm::vec2 inPercXY = { renderer.windowW, renderer.windowH };
	inPercXY -= glm::vec2(renderer.windowW, renderer.windowH) * inPerc;

	Ui::Frame frame({ inPercXY.x, inPercXY.y, renderer.windowW * inPerc, renderer.windowH * inPerc });
#pragma endregion

	float posy = 20;


	int count = 0;
	for(auto &i: perFrameData.lines)
	{
		bool isSelected = false;
		if(perMenuData.cursorIndex == count)
		{
			isSelected = true;
		}

		glm::vec4 textBox = Ui::Box().xLeft(20).yTop(posy);
		glm::vec2 size = renderer.getTextSize(i.text, f, 0.7);
		float oneLineSize = renderer.getTextSize("|", f, 0.7).y;
		textBox.z = size.x;
		textBox.w = size.y;
		auto p = platform::getRelMousePosition();

		bool mouseMoved = platform::mouseMoved();
		
		switch (i.type)
		{
		case i.uninteractableCentreText:
		{
			glm::vec4 center = Ui::Box().xCenter().yCenter()();
			renderer.renderText({ textBox.x, textBox.y }, i.text, f, uninteractebleTextColor,
				0.7, 4, 3, false, { 0.1,0.1,0.1,1 }, { 0.2,0.3,0.5,0.1 });
		}
			break;
		case i.booleanTextBox:
		{
			float bonusW = renderer.getTextSize(" : off ", f, 0.7, 4, 3).x + 20;

			glm::vec4 color = Colors::white;
			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW;

			if(!usingControllerInput)
			{
				if(Ui::isInButton(p,box) && mouseMoved)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				color = selectedButtonColor;
				textBox.x += 20;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7, 4, 3, false);


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
					0.7, 4, 3, false);
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
					0.7, 4, 3, false);
			}

			

			count++;
		}
			break;
		case i.slider0_1:
		{

			glm::vec4 color(1, 1, 1, 1);
			if (isSelected)
			{
				color = selectedButtonColor;
				textBox.x += 20;
			}

			float bonusW = renderer.getTextSize(" : < 100%  >", f, 0.7, 4, 3).x;

			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW + 30;


			float bonusLeft1 = renderer.getTextSize(" :", f, 0.7, 4, 3).x;
			float size = renderer.getTextSize(" < ", f, 0.7, 4, 3).x;
			bool leftArrowIn = Ui::isInButton(p,
				{ textBox.x + textBox.z + bonusLeft1, box.y, size, box.w });
			bool rightArrowIn = Ui::isInButton(p,
				{ textBox.x + textBox.z + bonusW - size, box.y, size, box.w });


			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && mouseMoved)
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
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });

			float v = *i.fVal * 100;

			int precision = 1;
			if (v == 100)
			{
				precision = 0;
			}else if ( v < 10)
			{
				precision = 2;
			}

			std::stringstream stream;
			stream << std::fixed << std::setprecision(precision) << v;
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
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
				bonus += renderer.getTextSize("  :", f, 0.7, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, "< ", f, leftColor,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
				bonus += renderer.getTextSize(" <", f, 0.7, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, (stream.str() + "% ").c_str(), f, color,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
				bonus += renderer.getTextSize((stream.str() + " %").c_str(), f, 0.7, 4, 3).x;
				renderer.renderText({ textBox.x + textBox.z + bonus, textBox.y }, ">", f, rightColor,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			}

			count++;
		}
			break;
		case i.interactableText:
		{
			float bonusW = renderer.getTextSize(" ", f, 0.7, 4, 3).x + 20;

			glm::vec4 box = textBox;
			box.y -= oneLineSize;
			box.z += bonusW;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && mouseMoved)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				textBox.x += 20;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, selectedButtonColor,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });

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
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			}

			count++;
		}
			break;
		default:
			break;
		}

		posy += 20 + textBox.w;

	}


	if (upReleased)
	{
		perMenuData.cursorIndex--;
	}
		
	if(downReleased)
	{
		perMenuData.cursorIndex++;
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

	renderer.currentCamera = c;

}
