#pragma once
#include "opengl2Dlib.h"


namespace settings
{

	struct SettingsData
	{
		float zoom = 0.5;
		float uiScale = 1;
		bool bShowArrowsIndicators = 1;
		float musicSound = 0.5;
		float ambientSound = 0.5;
		float buttonSound = 0.5;
		bool fullScreen = 0;
	};

void displaySettings(gl2d::Renderer2D &renderer, float deltaTime);

float getZoom();
float getUiScale();
bool showArrowIndicators();
void setMainSettingsPage();

float getMusicSound();
float getAmbientSound();
float getButtonSound();

bool isFullScreen();

void saveSettings();

void loadSettings();

SettingsData &getSettingsData();

};
