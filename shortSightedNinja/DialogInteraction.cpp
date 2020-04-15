#include "DialogInteraction.h"
#include "Ui.h"

extern gl2d::Texture uiDialogBox;
extern gl2d::Font font;

void DialogInteraction::setNewDialog(const char * newText)
{
	text = newText;
	mTextToShow.clear();
	mNextLetter = 0;
	mAnimTimePerLetter = 0;
	showing = 0;
	hasFinishedDialog = 0;
	mMoveAnimTime = 0;
}

float letterTime = 0.1;
float phrazeTime = 1.2;
float animTime = 2.2;

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
		if (mNextLetter < strlen(text))
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
		Ui::Frame f2({ w/10,h/10,w - w/10,h - h/10});
		glm::vec2 textPos = Ui::Box().xLeft(0).yTop(0)();

		if (!hasFinishedDialog)
			mTextToShow += '|';

		renderer.renderText(textPos,
			mTextToShow.c_str(), font, { 1,1,1,intensity }, 0.7, 4, 3, false);

		if (!hasFinishedDialog)
			mTextToShow.pop_back();
	
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
}
