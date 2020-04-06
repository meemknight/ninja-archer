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

void simulateLight(glm::vec2 pos, float radius, MapData &mapData, std::vector<glm::vec2> &triangles)
{
	triangles.clear();

	mapData.CalculateVisibilityPolygon(pos.x, pos.y, radius);

	auto it = std::unique(
		mapData.vecVisibilityPolygonPoints.begin(),
		mapData.vecVisibilityPolygonPoints.end(),
		[&](const VisibilityPolygonPoints& t1, const VisibilityPolygonPoints& t2)
		{
			return fabs(t1.x - t2.x) < 0.1f && fabs(t1.y - t2.y) < 0.1f;
		});
	mapData.vecVisibilityPolygonPoints.resize(std::distance(mapData.vecVisibilityPolygonPoints.begin(), it));

	if (mapData.vecVisibilityPolygonPoints.size() > 1)
	{
		// Draw each triangle in fan
		for(int i = 0; i < mapData.vecVisibilityPolygonPoints.size() - 1; i++)
		{
			drawLine({ pos.x, pos.y }, {mapData.vecVisibilityPolygonPoints[i].x,mapData.vecVisibilityPolygonPoints[i].y },
				{ 0.245f,0.189f,0.031f }, 3.0);

			drawLine({ pos.x, pos.y }, { mapData.vecVisibilityPolygonPoints[i + 1].x,mapData.vecVisibilityPolygonPoints[i + 1].y },
				{ 0.245f,0.189f,0.031f }, 3.0);

			drawLine({mapData.vecVisibilityPolygonPoints[i].x,mapData.vecVisibilityPolygonPoints[i].y },
				{ mapData.vecVisibilityPolygonPoints[i + 1].x,mapData.vecVisibilityPolygonPoints[i + 1].y },
				{ 0.245f,0.189f,0.031f }, 3.0);

		}

		drawLine({ pos.x, pos.y }, { mapData.vecVisibilityPolygonPoints[mapData.vecVisibilityPolygonPoints.size() - 1].x,
			mapData.vecVisibilityPolygonPoints[mapData.vecVisibilityPolygonPoints.size() - 1].y },
			{ 0.245f,0.189f,0.031f }, 3.0);
		drawLine({ pos.x, pos.y }, { mapData.vecVisibilityPolygonPoints[0].x, mapData.vecVisibilityPolygonPoints[0].y },
			{ 0.245f,0.189f,0.031f }, 3.0);
		drawLine({ mapData.vecVisibilityPolygonPoints[mapData.vecVisibilityPolygonPoints.size() - 1].x,
			mapData.vecVisibilityPolygonPoints[mapData.vecVisibilityPolygonPoints.size() - 1].y },
			{ mapData.vecVisibilityPolygonPoints[0].x, mapData.vecVisibilityPolygonPoints[0].y }, 
			{ 0.245f,0.189f,0.031f }, 3.0);
	}



	for (int x = 0; x < 40; x++)
	{
		for (int y = 0; y < 40; y++)
		{
			mapData.get(x, y).mainColor = { 1,1,1,1 };
		}
	}

#pragma region Debug the polygon edges
	/*for(auto &e: mapData.vecEdges)
	{
		drawLine({ e.sx, e.sy }, { e.ex, e.ey }, {0.066f,0.135f,0.245f}, 5.0);
		drawLine({ e.sx, e.sy }, { e.sx + 0.4, e.sy + 0.4 }, {0.245f,0.066f,0.066f},6.0);
		drawLine({ e.ex, e.ey }, { e.ex + 0.4, e.ey + 0.4 }, { 0.245f,0.066f,0.066f },6.0);
	}*/
#pragma endregion
}

void simuleteLightTrace(glm::vec2 pos, float radius, MapData & mapData, std::vector<glm::vec2>& triangles)
{
	float rotateStep = 0.1;
	float moveStep = BLOCK_SIZE * 0.1;

	for(float i=0; i< 3.141*2.f; i+= rotateStep)
	{
		glm::vec2 movePos = pos;
		glm::vec2 dir = { cos(i), -sin(i) };
		dir *= moveStep;
		
		for(int d=0; d<radius; d += moveStep)
		{
			movePos += dir;
			if(mapData.get(movePos.x / BLOCK_SIZE, movePos.y / BLOCK_SIZE).type == '!')
			{
				mapData.get(movePos.x / BLOCK_SIZE, movePos.y / BLOCK_SIZE).mainColor= glm::vec4(1, 1, 1, 1);
				break;
			}
		}
		drawLine( pos , movePos, { 0.245f,0.66f,0.66f }, 6.0);

	}


}

void simuleteLightSpot(glm::vec2 pos, float radius, MapData & mapData, std::vector<Arrow> &arrows, gl2d::Renderer2D &maskRenderer, gl2d::Texture lightT)
{
	//stencilRenderer.renderRectangle({ 3 * BLOCK_SIZE, 3 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 1,1,1,1 });

	maskRenderer.renderRectangle({ pos.x - radius*BLOCK_SIZE*2, pos.y - radius * BLOCK_SIZE*2, 4 * radius*BLOCK_SIZE, 4 * radius*BLOCK_SIZE }, {}, 0, lightT);

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
			l = sqrt(l);

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
