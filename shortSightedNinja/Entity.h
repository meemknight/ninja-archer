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

	int wallGrab = 0;

	bool grounded;
	bool movingRight;

	void checkCollisionBrute(glm::vec2 &pos, glm::vec2 lastPos, MapData &mapData,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch);
	void resolveConstrains(MapData &mapData);

	float notGrabTime = 0;

	void updateMove() 
	{
		if(lastPos.x - pos.x < 0)
		{
			movingRight = 1;
		}else if(lastPos.x - pos.x > 0)
		{
			movingRight = 0;
		}

		lastPos = pos; 
	}
	
	void strafe(int dir);

	void run(float speed);

	void airRun(float speed);

	void applyGravity(float deltaTime);

	void applyVelocity(float deltaTime);

	void checkGrounded(MapData &mapDat);

	void checkWall(MapData &mapData, int move);

	void jump();

	void jumpFromWall();

private:
	glm::vec2 performCollision(MapData &mapDat, glm::vec2 pos, glm::vec2 size, glm::vec2 delta,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch);
};

struct Arrow
{
	//pos is the tip of the arrrow
	glm::vec2 pos;
	glm::vec2 shootDir;

	void draw(gl2d::Renderer2D &renderer, gl2d::Texture t);

	void move(float deltaTime);

	void checkCollision();
	
	bool leftMap(int w, int h);

	bool stuckInWall = 0;
};
