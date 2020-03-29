#include "Entity.h"
#include "glm/glm.hpp"


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


void Entity::checkCollision(MapData & mapData)
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	glm::vec2 delta = pos - lastPos;

	glm::vec2 newPos = performCollision(mapData, { pos.x, lastPos.y }, {dimensions.x, dimensions.y}, { delta.x, 0 });
	pos = performCollision(mapData, { newPos.x, pos.y }, { dimensions.x, dimensions.y }, { 0, delta.y });

	
}

glm::vec2 Entity::performCollision(MapData & mapData, glm::vec2 pos, glm::vec2 size, glm::vec2 delta)
{
	
	for (int y = 0; y < mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			if (mapData.get(x, y).type == '!')
			{
				if(aabb({ pos,dimensions }, { x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }))
				{
					if (delta.x != 0) 
					{
						if(delta.x < 0) // moving left
						{
							pos.x = x * BLOCK_SIZE + BLOCK_SIZE;
						}else
						{
							pos.x = x * BLOCK_SIZE - dimensions.x;
						}
					}
					else
					{
						if(delta.y < 0) //moving up
						{
							pos.y = y * BLOCK_SIZE + BLOCK_SIZE;
						}else
						{
							pos.y = y * BLOCK_SIZE - dimensions.y;
						}
					}
				
				}
			}

		}

	return pos;
}
