#include "mapData.h"
#include <string>

void MapData::create(int w, int h, void* d = 0)
{
	data = new Block[w * h];

	if(d)
	{
		std::memcpy(data, d, w*h);
	}
}

MapData::Block & MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

void MapData::cleanup()
{

	if(data)
	{
		delete[] data;
	}
}
