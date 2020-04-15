#pragma once
#include "Entity.h"
#include "opengl2Dlib.h"
#include <string>
#include <unordered_map>

struct textureDataWithUV
{
	gl2d::Texture t;
	glm::vec4 uv;
};

struct DialogData
{
	std::string text;
	textureDataWithUV image;
};


struct DialogInteraction
{

	std::vector<DialogData> dialogData;
	int currentDialogPos = 0;
	
	void resetDialogData();

	bool showing = 0;
	
	///should be used to close dialog with input
	bool hasFinishedDialog = 0;

	std::string text;

	std::string mTextToShow;
	float mAnimTimePerLetter=0;
	int mNextLetter = 0;
	textureDataWithUV currentImage;

	//updates new data
	void setNewDialog(const std::string &newText, textureDataWithUV img);

	//draws dialog box
	void draw(gl2d::Renderer2D &renderer,int w, int h ,float deltaTime);

	void close();
	void start();

	//-1 starting, 0 nothing 1 closing
	int mState=0;
	float mMoveAnimTime = 0;

	bool blockMovement();

	//returns 0 if can exit
	bool updateDialog();

};

