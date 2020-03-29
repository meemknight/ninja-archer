#include "mapData.h"
#include <string>

void MapData::create(int w, int h, const char* d = 0)
{
	this->w = w;
	this->h = h;

	data = new BlockInfo[w * h];

	if(d)
	{
		for(int i=0; i<w*h; i++)
		{
			data[i].type = d[i];
			data[i].mainColor = glm::vec4(1, 1, 1, 1);
		}
	}

	//todo setup neighbours
}

BlockInfo & MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

void MapData::clearColorData()
{
	if(data)
	{
		for (int i = 0; i < w*h; i++)
		{
			data[i].mainColor = { 0,0,0,0 };
		}
	}
}

void MapData::cleanup()
{
	if(data)
	{
		delete[] data;
	}
}

void BlockInfo::resetColors()
{
	mainColor = { 0,0,0,0 };
}
