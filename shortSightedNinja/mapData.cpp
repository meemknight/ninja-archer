#include "mapData.h"
#include <string>
#include "mapRenderer.h"
#include <algorithm>

void MapData::create(int w, int h, unsigned short* d = 0)
{
	cleanup();

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

	setNeighbors();
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
			data[i].sideColors = { 0,0,0,0 };
			data[i].resetColors();
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

bool BlockInfo::hasNeighborLeft()
{
	return neighbors & 0b0000'0010;
}

bool BlockInfo::hasNeighborRight()
{
	return neighbors & 0b0000'0001;
}

bool BlockInfo::hasNeighborTop()
{
	return neighbors & 0b0000'1000;
}

bool BlockInfo::hasNeighborDown()
{
	return neighbors & 0b0000'0100;
}

void BlockInfo::resetColors()
{
	mainColor = { 0,0,0,0 };
	directionalLight = { 0,0,0,0 };
	heat = 1;
	sideColors = {};
}

void MapData::setNeighbors()
{
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			auto &n = get(x, y).neighbors;
			n = 0;

			if (y > 0) // top
			{
				if(isOpaque(get(x,y-1).type))
				{
					n |= 0b0000'1000;
				}
			}
			if (y < h-1) // bottom
			{
				if (isOpaque(get(x, y + 1).type))
				{
					n |= 0b0000'0100;
				}
			}
			if (x > 0) // left
			{
				if (isOpaque(get(x-1, y).type))
				{
					n |= 0b0000'0010;
				}
			}
			if (x < w - 1) // bottom
			{
				if (isOpaque(get(x+1, y).type))
				{
					n |= 0b0000'0001;
				}
			}

		}
	}
}

