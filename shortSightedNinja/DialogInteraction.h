#pragma once
#include "Entity.h"
#include "opengl2Dlib.h"

struct DialogInteraction
{

	bool showing = 0;
	
	///should be used to close dialog with input
	bool hasFinishedDialog = 0;

	const char* text;

	std::string mTextToShow;
	float mAnimTimePerLetter=0;
	int mNextLetter = 0;

	//updates new data
	void setNewDialog(const char* newText);

	//draws dialog box
	void draw(gl2d::Renderer2D &renderer,int w, int h ,float deltaTime);

	void close();
	void start();

	//-1 starting, 0 nothing 1 closing
	int mState=0;
	float mMoveAnimTime = 0;

};

