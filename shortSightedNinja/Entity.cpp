#include "Entity.h"
#include "glm/glm.hpp"

float gravitationalAcceleration = 64;
float jumpSpeed = 24;
float velocityClamp = 30;
float drag = 0.5f;

//pos and size
bool aabb(glm::vec4 b1, glm::vec4 b2)
{
	if (((b1.x - b2.x < b2.z)
		&& b2.x - b1.x < b1.z
		)
		&& ((b1.y - b2.y < b2.w)
			&& b2.y - b1.y < b1.w
			)
		)
	{
		return 1;
	}
	return 0;
}


void Entity::checkCollisionBrute(glm::vec2 pos, MapData & mapData)
{
	grounded = 0;

	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;

	glm::vec2 delta = pos - lastPos;
	glm::vec2 fullDelta = pos - lastPos;

	

	glm::vec2 newPos = performCollision(mapData, { pos.x, lastPos.y }, { dimensions.x, dimensions.y }, { delta.x, 0 },
		upTouch, downTouch, leftTouch, rightTouch);
	pos = performCollision(mapData, { newPos.x, pos.y }, { dimensions.x, dimensions.y }, { 0, delta.y },
		upTouch, downTouch, leftTouch, rightTouch);



	if(downTouch)
	{
		grounded = 1;
	}

	if(upTouch)
	{
		if(velocity.y<0)
		{
			velocity.y = 0;
		}
	}
}

void Entity::resolveConstrains(MapData & mapData)
{
	grounded = 0;

	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;

	glm::vec2 delta = pos - lastPos;
	glm::vec2 fullDelta = pos - lastPos;



	glm::vec2 newPos = performCollision(mapData, { pos.x, lastPos.y }, { dimensions.x, dimensions.y }, { delta.x, 0 },
		upTouch, downTouch, leftTouch, rightTouch);
	pos = performCollision(mapData, { newPos.x, pos.y }, { dimensions.x, dimensions.y }, { 0, delta.y },
		upTouch, downTouch, leftTouch, rightTouch);



	if (downTouch)
	{
		grounded = 1;
	}

	if (upTouch)
	{
		if (velocity.y < 0)
		{
			velocity.y = 0;
		}
	}
}

void Entity::applyGravity(float deltaTime)
{
	velocity.y += deltaTime * gravitationalAcceleration * BLOCK_SIZE;
}

void Entity::applyVelocity(float deltaTime)
{
	const float c = velocityClamp * BLOCK_SIZE;
	velocity = glm::clamp(velocity, { -c,-c }, { c, c });

	pos += velocity * deltaTime;

	//drag
	velocity.x += velocity.x * (-drag * deltaTime * BLOCK_SIZE);

	if (std::fabs(velocity.x) < 0.01)
	{
		velocity.x = 0;
	}

	if (std::fabs(velocity.y) < 0.01)
	{
		velocity.y = 0;
	}

	if (grounded && velocity.y > 0)
	{
		velocity.y = 0;
	}

}

void Entity::jump()
{
	if(grounded)
	{
		velocity.y = -jumpSpeed * BLOCK_SIZE;
	}
}

glm::vec2 Entity::performCollision(MapData & mapData, glm::vec2 pos, glm::vec2 size, glm::vec2 delta,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch)
{
	int minX=0;
	int minY=0;
	int maxX=mapData.w;
	int maxY=mapData.h;

	minX = (pos.x - abs(delta.x) - BLOCK_SIZE)/BLOCK_SIZE;
	maxX = ceil((pos.x + abs(delta.x) + BLOCK_SIZE + size.x)/BLOCK_SIZE);
	
	minY = (pos.y - abs(delta.y) - BLOCK_SIZE)/BLOCK_SIZE;
	maxY = ceil((pos.y + abs(delta.y) + BLOCK_SIZE + size.y)/BLOCK_SIZE);

	minX = max(0, minX);
	minY = max(0, minY);
	maxX = min(mapData.w, maxX);
	maxY = min(mapData.h, maxY);

	for (int y = minY; y < maxY; y++)
		for (int x = minX; x < maxX; x++)
		{
			if (mapData.get(x, y).type == '!')
			{
				if(aabb({ pos,dimensions }, { x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }))
				{
					if (delta.x != 0) 
					{
						if(delta.x < 0) // moving left
						{
							leftTouch |= 1;
							pos.x = x * BLOCK_SIZE + BLOCK_SIZE;
							goto end;
						}else
						{
							rightTouch |= 1;
							pos.x = x * BLOCK_SIZE - dimensions.x;
							goto end;
						}
					}
					else
					{
						if(delta.y < 0) //moving up
						{
							upTouch |= 1;
							pos.y = y * BLOCK_SIZE + BLOCK_SIZE;
							goto end;
						}else
						{
							downTouch |= 1;
							pos.y = y * BLOCK_SIZE - dimensions.y;
							goto end;
						}
					}
				
				}
			}

		}

	end:
	return pos;
}
