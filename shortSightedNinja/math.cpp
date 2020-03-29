#include "math.h"
#include "mapRenderer.h"

void simulateLight(glm::vec2 pos, MapData &mapData, std::vector<glm::vec2> &triangles)
{
	//mapData.get(0, 0).mainColor = { 1,1,1,1 }; light up
	//BLOCK_SIZE
	//isOpaque(mapData.get(0, 0).type);
	triangles.clear();

	for(int x=0; x<10; x++)
	{
		for(int y=0;y<10;y++)
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
