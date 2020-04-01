#include "Entity.h"
#include "glm/glm.hpp"

float gravitationalAcceleration = 64;
float jumpSpeed = 22;
float jumpFromWallSpeed = 10;
float velocityClamp = 30;
float drag = 0.15f;
float strafeSpeed = 10;
float runSpeed = 14;
float airRunSpeed = 10;

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


void Entity::checkCollisionBrute(glm::vec2 &pos, glm::vec2 lastPos, MapData & mapData,
	bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch)
{

	glm::vec2 delta = pos - lastPos;

	glm::vec2 newPos = performCollision(mapData, { pos.x, lastPos.y }, { dimensions.x, dimensions.y }, { delta.x, 0 },
		upTouch, downTouch, leftTouch, rightTouch);
	pos = performCollision(mapData, { newPos.x, pos.y }, { dimensions.x, dimensions.y }, { 0, delta.y },
		upTouch, downTouch, leftTouch, rightTouch);

}

void Entity::resolveConstrains(MapData & mapData)
{
	/*
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
	return;
	*/

	
	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;
	
	float distance = glm::length(lastPos - pos);
	
	if (distance < BLOCK_SIZE)
	{
		checkCollisionBrute(pos,
			lastPos,
			mapData,
			upTouch,
			downTouch,
			leftTouch,
			rightTouch
			);
	}
	else
	{
		glm::vec2 newPos = lastPos;
		glm::vec2 delta = pos - lastPos;
		delta = glm::normalize(delta);
		delta *= 0.9 * BLOCK_SIZE;
	
		do
		{
			newPos += delta;
			glm::vec2 posTest = newPos;
			checkCollisionBrute(newPos,
				lastPos,
				mapData,
				upTouch,
				downTouch,
				leftTouch,
				rightTouch);
	
			if (newPos != posTest)
			{
				pos = newPos;
				goto end;
			}
	
		} while (glm::length((newPos + delta) - pos) > 1.0f * BLOCK_SIZE);
		//todo optimize this while
	
		checkCollisionBrute(pos,
			lastPos,
			mapData,
			upTouch,
			downTouch,
			leftTouch,
			rightTouch);
	}
	
	end:
	
	if (upTouch)
	{
		if (velocity.y < 0)
		{
			velocity.y = 0;
		}
	}

}

void Entity::strafe(int dir)
{
	velocity.x = dir * strafeSpeed * BLOCK_SIZE;
}

void Entity::run(float speed)
{
	pos.x += speed * runSpeed * BLOCK_SIZE;
}

void Entity::airRun(float speed)
{
	pos.x += speed * airRunSpeed * BLOCK_SIZE;
}

void Entity::applyGravity(float deltaTime)
{
	if(wallGrab == 0)
	{
		velocity.y += deltaTime * gravitationalAcceleration * BLOCK_SIZE;
	}
}

void Entity::applyVelocity(float deltaTime)
{
	const float c = velocityClamp * BLOCK_SIZE;
	velocity = glm::clamp(velocity, { -c,-c }, { c, c });

	if(wallGrab != 0)
	{
		velocity.y = 0;
	}


	pos += velocity * deltaTime;

	//drag
	velocity.x += velocity.x * (-drag * deltaTime * BLOCK_SIZE);

	if(grounded || wallGrab)
	{
		velocity.x = 0;
	}

	if (std::fabs(velocity.x) < 10)
	{
		velocity.x = 0;
	}

	if (std::fabs(velocity.y) < 0.01)
	{
		velocity.y = 0;
	}

	if (grounded && velocity.y > 0)
	{
		velocity.y = 20;
	}

}

void Entity::checkGrounded(MapData &mapDat)
{
	grounded = 0;

	int minx = floor((pos.x + 1)/BLOCK_SIZE);
	int maxx = floor((pos.x + dimensions.x - 1)/BLOCK_SIZE);

	minx = max(minx, 0);
	maxx = min(maxx, mapDat.w);


	for(int x=minx; x<=maxx; x++)
	{
		if(isColidable(mapDat.get(x, floor((pos.y +dimensions.y + 0.1)/BLOCK_SIZE)).type))
		{
			grounded = 1;
			break;
		}
	}

}

void Entity::checkWall(MapData & mapData, int move)
{
	if(grounded)
	{
		return;
	}

	int minY = floor((pos.y /BLOCK_SIZE)+0.1f);
	int maxY = minY + 1;
	int rightX = floor((pos.x + dimensions.x) / BLOCK_SIZE);
	int leftX = floor((pos.x-2) / BLOCK_SIZE);

	//llog(minY, rightX);

	//for(int y= minY; y<maxY; y++)
	//{
	if(isColidable(mapData.get(rightX, minY).type) && move > 0)
	{
		wallGrab = 1;
	}
	if (isColidable(mapData.get(leftX, minY).type) && move < 0)
	{
		wallGrab = -1;
	}
	//}


}

void Entity::jump()
{
	velocity.y = -jumpSpeed * BLOCK_SIZE;
}

void Entity::jumpFromWall()
{
	velocity.y = -jumpFromWallSpeed * BLOCK_SIZE;
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
							leftTouch = 1;
							pos.x = x * BLOCK_SIZE + BLOCK_SIZE;
							goto end;
						}else
						{
							rightTouch = 1;
							pos.x = x * BLOCK_SIZE - dimensions.x;
							goto end;
						}
					}
					else
					{
						if(delta.y < 0) //moving up
						{
							upTouch = 1;
							pos.y = y * BLOCK_SIZE + BLOCK_SIZE;
							goto end;
						}else
						{
							downTouch = 1;
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
