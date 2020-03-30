#include "math.h"
#include "mapRenderer.h"

extern gl2d::Renderer2D renderer2d;

void drawLine(glm::vec2 x, glm::vec2 y)
{
	y = y - x;
	auto pos = renderer2d.toScreen({ x.x, x.y, y.x, y.y });
	glLineWidth(5.0);
	
	glUseProgram(0);

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.5f, 0.5f);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.z, pos.w);
	glEnd();

}

void simulateLight(glm::vec2 pos, MapData &mapData, std::vector<glm::vec2> &triangles)
{
	//mapData.get(0, 0).mainColor = { 1,1,1,1 }; light up
	//BLOCK_SIZE
	//isOpaque(mapData.get(0, 0).type);
	triangles.clear();

	
	drawLine({100,100}, {200,200});


	for(int x=0; x<40; x++)
	{
		for(int y=0;y<40;y++)
		{
			mapData.get(x, y).mainColor = { 1,1,1,1 };
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
