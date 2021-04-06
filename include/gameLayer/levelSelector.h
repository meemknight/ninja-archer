#pragma once
#include <gl2d/gl2d.h>
#define LEVELS 7

int levelSelectorMenu(float deltaTime, gl2d::Renderer2D &renderer2d, gl2d::Texture &uiDialogBox, gl2d::Font &font);

void initLevelSelectorData();