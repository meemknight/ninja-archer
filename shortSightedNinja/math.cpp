#include "math.h"
#include "mapRenderer.h"
#include <algorithm>
#include "math.h"
#include <glm/glm.hpp>
#include "Entity.h"

#undef max

extern gl2d::Renderer2D renderer2d;

void drawLine(glm::vec2 x, glm::vec2 y, glm::vec3 color, float width)
{
	y = y - x;
	auto pos = renderer2d.toScreen({ x.x, x.y, y.x, y.y });
	glLineWidth(width);
	
	glUseProgram(0);

	glBegin(GL_LINES);
	glColor3f(color.x, color.y, color.z);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.z, pos.w);
	glEnd();

}

void simuleteLightSpot(glm::vec2 pos, float radius, MapData & mapData)
{
	//stencilRenderer.renderRectangle({ 3 * BLOCK_SIZE, 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 1,1,1,1 });


	for (int y = -radius; y <= radius; y += 1)
	{
		for (int x = -radius; x <= radius; x += 1)
		{
			int xPos = x + (pos.x / BLOCK_SIZE);
			int yPos = y + (pos.y / BLOCK_SIZE);
			float dist = x * x + y * y;
			float maxDist = radius * radius;
			if (dist <= maxDist)
			{
				if (xPos >= 0 && yPos >= 0 && xPos < mapData.w && yPos < mapData.h)
				{
					float perc = dist / maxDist;
					mapData.get(xPos, yPos).mainColor = glm::max(mapData.get(xPos, yPos).mainColor,
						glm::vec4{ 1 - perc,1 - perc,1 - perc,1 });

					float top = 0;
					float bottom = 0;
					float left = 0;
					float right = 0;

					glm::vec2 dirVect = glm::vec2{ xPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f) ,yPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f) } -glm::vec2{ pos.x , pos.y };
					dirVect = glm::normalize(dirVect);

					if (pos.y < yPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f))
					{
						glm::vec2 normalVect{ 0,1 };

						top = glm::dot(normalVect, dirVect) * (1 - perc);
					}

					if (pos.y > yPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f))
					{
						glm::vec2 normalVect{ 0,-1 };

						bottom = glm::dot(normalVect, dirVect) * (1 - perc);
					}

					if (pos.x > xPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f))
					{
						glm::vec2 normalVect{ -1,0 };

						right = glm::dot(normalVect, dirVect) * (1 - perc);
					}

					if (pos.x < xPos * BLOCK_SIZE + (BLOCK_SIZE / 2.f))
					{
						glm::vec2 normalVect{ 1,0 };

						left = glm::dot(normalVect, dirVect) * (1 - perc);
					}

					mapData.get(xPos, yPos).sideColors = glm::max(mapData.get(xPos, yPos).sideColors,
						glm::vec4{ top,bottom,left,right });

					static int c;
				}
				
			}
			

		}

	}


}

/////////////////
//(0,0)  -> (16, 0)
//
//	
//
//
//
//
