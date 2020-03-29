#pragma once
#include "glm/vec2.hpp"
#include "mapData.h"
#include "mapRenderer.h"

struct Entity
{
	glm::vec2 pos;
	glm::vec2 lastPos;

	glm::vec2 dimensions;

	void checkCollision(MapData &mapData);
	
	void updateMove() { lastPos = pos; }
	

private:
	glm::vec2 performCollision(MapData &mapDat, glm::vec2 pos, glm::vec2 size, glm::vec2 delta);
};

