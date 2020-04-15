#include "DialogInteraction.h"
#include "Ui.h"

extern gl2d::Texture uiDialogBox;
extern gl2d::Font font;
extern std::unordered_map<std::string, textureDataWithUV> textureDataForDialog;

void DialogInteraction::resetDialogData()
{
	dialogData.clear();
	currentDialogPos = 0;
	mTextToShow.clear();
	mNextLetter = 0;
	text = nullptr;
}

void DialogInteraction::setNewDialog(const std::string &newText, textureDataWithUV img)
{
	text = newText;
	mTextToShow.clear();
	mNextLetter = 0;
	currentImage = img;
}

float letterTime = 0.07;
float phrazeTime = 1;
float animTime = 1.8;

float getPerc(float time)
{
	return time / animTime;
}

void DialogInteraction::draw(gl2d::Renderer2D &renderer, int w, int h, float deltaTime)
{
	if(!showing && mState == 0)
	{
		return;
	}

	float hModifier = 1;

	if(mState != 0)
	{
		mMoveAnimTime -= deltaTime;
		if(mMoveAnimTime <=0)
		{
			mMoveAnimTime = 0;
			
			if(mState == 1)
			{
				return;
			}
			mState = 0;
		}else
		{
			if (mState == -1)
			{
				hModifier = getPerc(mMoveAnimTime);
				hModifier = 1 - hModifier;
			}
			else if(mState == 1)
			{
				hModifier = getPerc(mMoveAnimTime);
			}
		}
	}

	if(mState == 0)
	{
		hasFinishedDialog = true;
		if (mNextLetter < text.size())
		{
			hasFinishedDialog = false;
			mAnimTimePerLetter -= deltaTime;

			if (mAnimTimePerLetter < 0)
			{
				mAnimTimePerLetter += letterTime;
				mTextToShow += text[mNextLetter];
				if (text[mNextLetter] == '.')
				{
					mAnimTimePerLetter = phrazeTime;
				}
				mNextLetter++;
			}

		}
	}

	

#pragma region draw

	auto c = renderer.currentCamera;
	renderer.currentCamera.setDefault();

	Ui::Frame f({ 0,0,w,(h*hModifier) / 2.5 });

	float intensity = 0.7;

	//todo remke 9patches
	renderer.render9Patch2(Ui::Box().xRight().yTop().xDimensionPercentage(1).yDimensionPercentage(1),
		10,
		{1,1,1,intensity}, {}, 0,
		uiDialogBox, {0,1,1,0},
		{0,0.8,0.8,0}
	);

	if(mState == 0)
	{
		{
			Ui::Frame f2({ w / 10,h / 10,w/2 - w / 10, h/2 - h / 10 });

			renderer.renderRectangle(
				Ui::Box().xCenter().yCenter().yDimensionPercentage(1).xAspectRatio(1),
				{}, 0, currentImage.t, currentImage.uv
			);

		}

		{
			Ui::Frame f2({ w / 2,h / 10,w/2 - w / 10,h - h / 10 });

			glm::vec2 textPos = Ui::Box().xLeft(0).yTop(0)();

		if (!hasFinishedDialog)
			mTextToShow += '|';

		renderer.renderText(textPos,
			mTextToShow.c_str(), font, { 1,1,1,intensity }, 0.7, 4, 3, false);

		if (!hasFinishedDialog)
			mTextToShow.pop_back();

		}
	}
	renderer.currentCamera = c;
#pragma endregion

}

void DialogInteraction::close()
{
	mState = 1;
	showing = 0;
	if(mMoveAnimTime <=0)
	{
		mMoveAnimTime = animTime;
	}
}

void DialogInteraction::start()
{
	mState = -1;
	showing = 1;
	if (mMoveAnimTime <= 0)
	{
		mMoveAnimTime = animTime;
	}


	if (dialogData.size())
	{
		
		setNewDialog(dialogData[currentDialogPos].text, dialogData[currentDialogPos].image);

		currentDialogPos++;
	}

}

bool DialogInteraction::blockMovement()
{
	return (mState != 0 || showing);
}

bool DialogInteraction::updateDialog()
{

	if (currentDialogPos < dialogData.size())
	{

		setNewDialog(dialogData[currentDialogPos].text, dialogData[currentDialogPos].image);
		currentDialogPos++;
		return true;

	}else
	{
		return 0;
	}

}
