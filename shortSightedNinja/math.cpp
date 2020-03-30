#include "math.h"
#include "mapRenderer.h"
#include <algorithm>

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



/////////////////
//(0,0)  -> (16, 0)
//
//	
//
//
//
//
