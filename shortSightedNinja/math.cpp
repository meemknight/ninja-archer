#include "math.h"
#include "mapRenderer.h"
#include <algorithm>
#include "math.h"
#include <glm/glm.hpp>
#include "Entity.h"

#undef max
#undef min

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

void simuleteLightSpot(glm::vec2 pos, float radius, MapData & mapData, std::vector<Arrow> &arrows, std::vector<Pickup> &pickups, float heat)
{
	//stencilRenderer.renderRectangle({ 3 * BLOCK_SIZE, 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 1,1,1,1 });

	//xmaskRenderer.renderRectangle({ pos.x - radius*BLOCK_SIZE*2, pos.y - radius * BLOCK_SIZE*2, 4 * radius*BLOCK_SIZE, 4 * radius*BLOCK_SIZE }, {}, 0, lightT);

	{
		float r = radius;
		float maxDist = r * r *BLOCK_SIZE * BLOCK_SIZE;
		for (auto &i : arrows)
		{

			float x = pos.x - i.pos.x;
			float y = pos.y - i.pos.y;
			float dist = x * x + y * y;

			float perc = dist / maxDist;
			float l = (1 - perc);

			if (l < 0) { l = 0; }

			i.light = std::max(i.light, l);
		}
	}

	{
		float r = radius ;
		float maxDist = r * r *BLOCK_SIZE * BLOCK_SIZE;
		for (auto &i : pickups)
		{
			float x = pos.x - i.pos.x*BLOCK_SIZE;
			float y = pos.y - i.pos.y*BLOCK_SIZE;
			float dist = x * x + y * y;

			float perc = dist / maxDist;
			float l = (1 - perc);

			if (l < 0) { l = 0; }
			l = pow(l,0.9);

			i.light = std::max(i.light, l);
		}
	}

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
					glm::vec4 color= { 1 - perc,1 - perc,1 - perc,1 };
					
					{
						float heatPerc = std::max(0.08f, perc);
						heatPerc = std::pow(heatPerc, heat);
						mapData.get(xPos, yPos).heat = std::min( heatPerc, mapData.get(xPos, yPos).heat );
					}

					mapData.get(xPos, yPos).mainColor = glm::max(mapData.get(xPos, yPos).mainColor,
						color);

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

					top = sqrt(top);
					bottom = sqrt(top);
					left = sqrt(left);
					right = sqrt(right);

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
