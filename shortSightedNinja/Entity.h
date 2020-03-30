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

	bool grounded;
	bool movingRight;

	void checkCollision(MapData &mapData);
	
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
	
	void applyGravity(float deltaTime);

	void applyVelocity(float deltaTime);

	void jump();

private:
	glm::vec2 performCollision(MapData &mapDat, glm::vec2 pos, glm::vec2 size, glm::vec2 delta);
};

