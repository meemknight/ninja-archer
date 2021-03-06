#include "mapData.h"
#include <string>
//#include "mapRenderer.h"
#include <algorithm>
#include "Entity.h"


#undef min;
#undef max;

float distFunc(float dist)
{

	dist /= BLOCK_SIZE;

	dist /= 1.5;

	dist -= 6;

	dist = std::max(1.f, dist);
	//shortestDist /= BLOCK_SIZE;

	float perc = (100.f * BLOCK_SIZE) / (pow(dist, 2) * 0.04 + 3 + pow(dist, 3) * 0.008);
	perc = std::min(perc, 100.f);
	perc = std::max(perc, 0.f);

	if (perc < 2)
	{
		perc = 0;
	}

	if (perc * 0.08 < 1.62) { perc = 0; };

	return perc * 0.08 * 0.6;
}


void MapData::create(int w, int h, unsigned short* d = 0)
{
	cleanup();

	this->w = w;
	this->h = h;

	data = new BlockInfo[w * h];

	if (d)
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].type = d[i];
			data[i].mainColor = 1;

		}
	}

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			auto& d = get(x, y);
			
		
			if(d.type == Block::butterfy)
			{
				butterflies.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
				d.type = Block::none;
			}else 
			if (d.type == Block::fireFly)
			{
				butterflies.push_back({ glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f },
					Butterfly::fireFlyType });
				d.type = Block::none;
			}else
			if (d.type == Block::craw)
			{
				craws.push_back({ glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f } });
				d.type = Block::none;
			}
		}

	setNeighbors();
}

BlockInfo& MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

float MapData::getTorchLightIntensity(int x, int y)
{
	auto iter = std::find_if(torchDataVector.begin(), torchDataVector.end(),
		[x, y](torchData &d)->bool {return (d.pos.x == x && d.pos.y == y); });

	if (iter != torchDataVector.end())
	{
		return iter->light;
	}else
	{
		return 5;
	}
}

torchData MapData::getTorchData(int x, int y)
{
	auto iter = std::find_if(torchDataVector.begin(), torchDataVector.end(),
		[x, y](torchData &d)->bool { return (d.pos.x == x && d.pos.y == y); });

	if (iter != torchDataVector.end())
	{
		return *iter;
	}
	else
	{
		return torchData({x, y}, 5, 0, 0);
	}
}

void MapData::clearColorData()
{
	if (data)
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].resetColors();
		}
	}
}

void MapData::cleanup()
{
	
	signDataVector.clear();
	
	exitDataVector.clear();
	torchDataVector.clear();
	dialogs.clear();
	butterflies.clear();
	craws.clear();

	if (data)
	{
		delete[] data;
		data = nullptr;
	}

	w = 0;
	h = 0;
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
	mainColor = 0;
	sideColors = {};
	directionalLight = { 0,0,0,0 };
	sideColors = {};
}

void MapData::setNeighbors()
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			auto& n = get(x, y).neighbors;
			n = 0;

			if (y > 0) // top
			{
				if (isOpaque(get(x, y - 1).type))
				{
					n |= 0b0000'1000;
				}
			}
			if (y < h - 1) // bottom
			{
				if (isOpaque(get(x, y + 1).type))
				{
					n |= 0b0000'0100;
				}
			}
			if (x > 0) // left
			{
				if (isOpaque(get(x - 1, y).type))
				{
					n |= 0b0000'0010;
				}
			}
			if (x < w - 1) // bottom
			{
				if (isOpaque(get(x + 1, y).type))
				{
					n |= 0b0000'0001;
				}
			}

		}
	}
}

