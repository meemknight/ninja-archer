#include "levelSelector.h"
#include "menu.h"
#include "mapData.h"

const char *levelNames[LEVELS] = { "Tutorial", "Enchanted forest", "Cave", "Tiki tribe", "Secret Level", "test world", "shire" };

int levelSelectorMenu(float deltaTime, gl2d::Renderer2D &renderer2d, gl2d::Texture &uiDialogBox, gl2d::Font &font)
{
	menu::startMenu(1020);

	menu::uninteractableCentreText("Select zone");

	bool selected[LEVELS] = {};

	for (int i = 0; i < LEVELS; i++)
	{
		menu::interactableText(levelNames[i], &selected[i]);
	}

	menu::endMenu(renderer2d, uiDialogBox, font, nullptr, deltaTime);
	
	for (int i = 0; i < LEVELS; i++)
	{
		if (selected[i])
		{
			return i;
		}
	}

	return -1;
}
