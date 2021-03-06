#include "Settings.h"
#include "gl2d.h"
#include "menu.h"
#include "gameLayer.h"
#include "input.h"

extern gl2d::Texture uiDialogBox;
extern gl2d::Texture uiBackArrow;
extern gl2d::Font font;

int currentSettingsMenu = 0;

settings::SettingsData sData;

namespace settings {

	float getZoom()
	{
		return 1 + sData.zoom * 1;
	}

	float getUiScale()
	{
		return 3 + sData.zoom * 4;
	}

	bool showArrowIndicators()
	{
		return sData.bShowArrowsIndicators;
	}

	void setMainSettingsPage()
	{
		currentSettingsMenu = 1;
	}

	void setControllsPage()
	{
		currentSettingsMenu = 4;
	}

	float getMusicSound()
	{
		return sData.musicSound / 2.f;
	}

	float getAmbientSound()
	{
		return sData.ambientSound * 2;
	}

	float getButtonSound()
	{
		return sData.buttonSound;
	}

	bool isFullScreen()
	{
		return sData.fullScreen;
	}

	SettingsData &getSettingsData()
	{
		return sData;
	}

	void saveSettings()
	{

		platform::writeEntireFile(RESOURCES_PATH "settings",
			(void*)&settings::getSettingsData(), sizeof(settings::getSettingsData()));

	}

	void loadSettings()
	{
		if (!platform::readEntireFile(RESOURCES_PATH "settings",
			(void*)&settings::getSettingsData(), sizeof(settings::getSettingsData())))
		{
			settings::getSettingsData() = SettingsData();
		}

	}


	void displaySettings(gl2d::Renderer2D &renderer, float deltaTime)
	{
		auto c = renderer.currentCamera;

		renderer.currentCamera.setDefault();

		if (currentSettingsMenu == 1)
		{
			menu::startMenu(2);

			menu::uninteractableCentreText("Settings page");

			bool visualSettings = 0;
			menu::interactableText("Visual settings", &visualSettings);

			bool soundSettings = 0;
			menu::interactableText("sound settings", &soundSettings);

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, uiBackArrow, font, &backPressed, deltaTime);

			if (visualSettings == true)
			{
				currentSettingsMenu = 2;
			}
			if (soundSettings == true)
			{
				currentSettingsMenu = 3;
			}
			if (backPressed)
			{
				currentSettingsMenu = 0;
			}
		}
		else if (currentSettingsMenu == 2)
		{
			bool pressedWindowed = 0;
			bool pressedFullScreen = 0;

			menu::startMenu(3);

			menu::uninteractableCentreText("Visual settings");

			//if(!settings::isFullScreen())
			{
				menu::slider0_1("Zoom", &sData.zoom);
			}

			menu::slider0_1("Ui scale", &sData.uiScale);

			if(sData.fullScreen)
			{
				menu::interactableText("Go windowed", &pressedWindowed);
				

			}else
			{
				menu::interactableText("Go full screen", &pressedFullScreen);
				
			}

			menu::booleanTextBox("Show arrow switch\n buttons", &sData.bShowArrowsIndicators);


			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, uiBackArrow, font, &backPressed, deltaTime);

			if (backPressed)
			{
				currentSettingsMenu = 1;
			}
			if (pressedWindowed)
			{
				sData.fullScreen = 0;
			}
			if (pressedFullScreen)
			{
				sData.fullScreen = 1;
			}
		}
		else if (currentSettingsMenu == 3)
		{

			menu::startMenu(4);

			menu::uninteractableCentreText("Sound settings");

			menu::slider0_1("Music", &sData.musicSound);
			menu::slider0_1("Ambient", &sData.ambientSound);
			menu::slider0_1("Buttons", &sData.buttonSound);

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, uiBackArrow, font, &backPressed, deltaTime);

			if (backPressed)
			{
				currentSettingsMenu = 1;
			}


		}
		else if (currentSettingsMenu == 4)
		{


		}
		else
		{
			currentSettingsMenu = 0;
		}

		renderer.currentCamera = c;

	}


};
//		 	 //		 	 //
//		 	 //		 	 //
//	   1   	 //	   2  	 //
//	settings //	visual   //
//		 	 //		 	 //
//		 	 //	  3 	 //
//		 	 //	sound	 //
//		 	 //		 	 //
//		 	 //	  4    	 //
//		 	 //	controls //
//		 	 //		 	 //
//		 	 //		 	 //


