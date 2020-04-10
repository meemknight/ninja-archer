#pragma once
#include "glm/vec2.hpp"
#include "mapData.h"
#include "mapRenderer.h"


struct Entity
{
	glm::vec2 pos;
	glm::vec2 lastPos;

	glm::vec2 dimensions;

	glm::vec2 velocity;

	bool redGrab=0;
	bool blueGrab=0;
	bool grayGrab=0;
	int wallGrab = 0;
	int lastWallGrab = 0;

	bool grounded;
	bool movingRight;

	//this are related
	bool hasTouchGround;
	bool canJump;
	float timeLeftGrounded;

	float frameDuration = 0.10f;
	float currentCount = 0;
	int currentFrame = 0;

	float idleTime = 0;

	void checkCollisionBrute(glm::vec2 &pos, glm::vec2 lastPos, MapData &mapData,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch);
	void resolveConstrains(MapData &mapData);

	float notGrabTime = 0;

	bool moving = 0;
	bool dying = 0;
	bool lockMovementDie = 0;
	int isExitingLevel = -1;

	bool iswebs;

	void updateMove() 
	{
		if(lastPos.x - pos.x < 0)
		{
			movingRight = 1;
		}else if(lastPos.x - pos.x > 0)
		{
			movingRight = 0;
		}

		if(wallGrab == -1)
		{
			movingRight = 0;
		}
		if (wallGrab == 1)
		{
			movingRight = 1;
		}

		lastPos = pos; 
	}
	
	void strafe(int dir);

	void run(float speed);

	void airRun(float speed);

	void applyGravity(float deltaTime);

	void applyVelocity(float deltaTime);

	void checkGrounded(MapData &mapDat, float deltaTime);

	void checkWall(MapData &mapData, int move);

	void jump();

	void draw(gl2d::Renderer2D &renderer, float deltaTime, gl2d::Texture characterSprite);

	void jumpFromWall();

private:
	glm::vec2 performCollision(MapData &mapDat, glm::vec2 pos, glm::vec2 size, glm::vec2 delta,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch);
};

struct Arrow
{
	enum ArrowTypes: int
	{
		normalArrow=0,
		fireArrow,
		slimeArrow,
		keyArrow,
		bombArrow,
		lastArror,
	}type = normalArrow;


	//pos is the tip of the arrrow
	glm::vec2 pos;
	glm::vec2 lastPos;
	glm::vec2 shootDir;

	void draw(gl2d::Renderer2D &renderer, gl2d::Texture t);

	void move(float deltaTime);

	void checkCollision(MapData &mapData, bool redTouch, bool blueTouch, bool grayTouch);
	
	bool leftMap(int w, int h);

	bool timeOut(float deltaTime);

	float light = 1;

	float liveTime = 5;

	bool hitOnce = 0;
	bool stuckInWall = 0;
	bool shownAnim = 0;
};

struct Pickup
{
	Pickup() {};
	Pickup(int x, int y, int type) { pos.x = x, pos.y = y, this->type = type; };

	float animPos = 0;

	float light = 1;

	int type = 1;

	float cullDown = 0;

	glm::ivec2 pos;

	bool colidePlayer(Entity &player);

	void draw(gl2d::Renderer2D &renderer2d, gl2d::Texture arrowTexture, float deltaTime);

};