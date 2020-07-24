#pragma once
#include "opengl2Dlib.h"

namespace settings
{

void displaySettings(gl2d::Renderer2D &renderer, float deltaTime);

float getZoom();
float getUiScale();
bool showArrowIndicators();
void setMainSettingsPage();

float getMusicSound();
float getAmbientSound();
float getButtonSound();

bool isFullScreen();

};
