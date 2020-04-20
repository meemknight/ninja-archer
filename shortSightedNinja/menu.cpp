#include "menu.h"
#include <vector>

void menu::startMenu()
{
	perFrameData = {};
}

struct
{
	int cursorIndex = 0;

}perMenuData;


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
	glm::vec4 frame;
	float posY;
	std::vector <lineInfo> lines;

}perFrameData;


void menu::resetMenuState()
{
	perMenuData = {};
}

void menu::uninteractableCentreText(char * text)
{
	lineInfo info = {};
	info.type = info.uninteractableCentreText;
	info.text = text;
	perFrameData.lines.push_back(info);
}

void menu::booleanTextBox(char * text, bool * b)
{
	lineInfo info = {};
	info.type = info.booleanTextBox;
	info.text = text;
	info.bVal = b;
	perFrameData.lines.push_back(info);

}

void menu::slider0_1(char * text, float *val)
{
	lineInfo info = {};
	info.type = info.slider0_1;
	info.text = text;
	info.fVal = val;
	perFrameData.lines.push_back(info);
}

void menu::interactableText(char * text, bool *hasPressed)
{
	lineInfo info = {};
	info.type = info.interactableText;
	info.text = text;
	info.bVal = hasPressed;
	perFrameData.lines.push_back(info);
}

void menu::endMenu(gl2d::Renderer2D & renderer, gl2d::Texture backgroundTexture, gl2d::Font f, bool * backPressed)
{
}
