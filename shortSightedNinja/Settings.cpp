#include "Settings.h"
#include "opengl2Dlib.h"
#include "menu.h"

extern gl2d::Texture uiDialogBox;
extern gl2d::Font font;

int currentSettingsMenu = 0;
float zoom = 0.5;
float uiScale = 1;
bool bShowArrowsIndicators = 1;
float musicSound=0.5;
float ambientSound = 0.5;
float buttonSound=0.5;

namespace settings {

	float getZoom()
	{
		return 3 + zoom * 4;
	}

	float getUiScale()
	{
		return 3 + zoom * 4;
	}

	bool showArrowIndicators()
	{
		return bShowArrowsIndicators;
	}

	void setMainSettingsPage()
	{
		currentSettingsMenu = 1;
	}

	float getMusicSound()
	{
		return musicSound;
	}

	float getAmbientSound()
	{
		return ambientSound;
	}

	float getButtonSound()
	{
		return buttonSound;
	}

	void displaySettings(gl2d::Renderer2D &renderer, float deltaTime)
	{
		auto c = renderer.currentCamera;

		renderer.currentCamera.setDefault();

		if (currentSettingsMenu == 1)
		{
			menu::startMenu();

			menu::uninteractableCentreText("Settings page");

			bool visualSettings = 0;
			menu::interactableText("Visual settings", &visualSettings);

			bool soundSettings = 0;
			menu::interactableText("sound settings", &soundSettings);

			bool showControlls;
			menu::interactableText("Controlls", &showControlls);

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, font, &backPressed, deltaTime);

			if (visualSettings == true)
			{
				currentSettingsMenu = 2;
			}
			if (soundSettings == true)
			{
				currentSettingsMenu = 3;
			}
			if (showControlls == true)
			{
				currentSettingsMenu = 4;
			}
			if (backPressed)
			{
				currentSettingsMenu = 0;
			}
		}
		else if (currentSettingsMenu == 2)
		{
			menu::startMenu();

			menu::uninteractableCentreText("Visual settings");

			menu::slider0_1("Zoom", &zoom);
			menu::slider0_1("Ui scale", &uiScale);
			menu::booleanTextBox("Show arrow switch\n buttons", &bShowArrowsIndicators);

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, font, &backPressed, deltaTime);

			if (backPressed)
			{
				currentSettingsMenu = 1;
			}

		}
		else if (currentSettingsMenu == 3)
		{

			menu::startMenu();

			menu::uninteractableCentreText("Sound settings");

			menu::slider0_1("Music", &musicSound);
			menu::slider0_1("Ambient", &ambientSound);
			menu::slider0_1("Buttons", &buttonSound);

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, font, &backPressed, deltaTime);

			if (backPressed)
			{
				currentSettingsMenu = 1;
			}


		}
		else if (currentSettingsMenu == 4)
		{

			menu::startMenu();

			menu::uninteractableCentreText("Controlls");

			bool backPressed = 0;
			menu::endMenu(renderer, uiDialogBox, font, &backPressed, deltaTime);

			if (backPressed)
			{
				currentSettingsMenu = 1;
			}

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


