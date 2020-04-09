#include "mapData.h"
#include <string>
#include "mapRenderer.h"
#include <algorithm>

#undef min;
#undef max;

float distFunc(float dist)
{
	dist /= BLOCK_SIZE;

	dist = std::max(1.f, dist);
	//shortestDist /= BLOCK_SIZE;

	float perc = (100.f * BLOCK_SIZE) / (pow(dist, 2) * 0.2 + 3 + pow(dist, 3) * 0.012);
	perc = std::min(perc, 100.f);
	perc = std::max(perc, 0.f);

	if (perc < 2)
	{
		perc = 0;
	}

	return perc *0.2;
}

float MapData::getWaterPercentage(glm::vec2 pos)
{
	float shortestDist = -1;

	for (auto& i : waterPos)
	{
		float dist = sqrt(
		(pos.x - i.x) * (pos.x - i.x) +
		(pos.y - i.y) * (pos.y - i.y)
			);

		if (shortestDist < 0)
		{
			shortestDist = dist;
		}
		else
		{
			shortestDist = std::min(shortestDist, dist);
		}
	}

	if (shortestDist < 0)
		return 0;

	return distFunc(shortestDist) * 0.5;
}

float MapData::getGreenPercentage(glm::vec2 pos)
{
	float shortestDist = -1;

	for (auto& i : greenSoundPos)
	{
		float dist = sqrt(
		(pos.x - i.x) * (pos.x - i.x) +
		(pos.y - i.y) * (pos.y - i.y)
			);

		if (shortestDist < 0)
		{
			shortestDist = dist;
		}
		else
		{
			shortestDist = std::min(shortestDist, dist);
		}
	}

	if (shortestDist < 0)
		return 0;

	return distFunc(shortestDist) * 0.5;
}

float MapData::getTikiPercentage(glm::vec2 pos)
{
	float shortestDist = -1;

	for (auto& i : tikiSoundPos)
	{
		float dist = sqrt(
		(pos.x - i.x) * (pos.x - i.x) +
		(pos.y - i.y) * (pos.y - i.y)
			);

		if (shortestDist < 0)
		{
			shortestDist = dist;
		}
		else
		{
			shortestDist = std::min(shortestDist, dist);
		}
	}

	if (shortestDist < 0)
		return 0;

	return distFunc(shortestDist) * 0.5;
}

float MapData::getSnowPercentage(glm::vec2 pos)
{
	float shortestDist = -1;

	for (auto& i : snowSoundPos)
	{
		float dist = sqrt(
		(pos.x - i.x) * (pos.x - i.x) +
		(pos.y - i.y) * (pos.y - i.y)
			);

		if (shortestDist < 0)
		{
			shortestDist = dist;
		}
		else
		{
			shortestDist = std::min(shortestDist, dist);
		}
	}

	if (shortestDist < 0)
		return 0;

	return distFunc(shortestDist) * 0.5;
}

float MapData::getCavePercentage(glm::vec2 pos)
{
	float shortestDist = -1;

	for (auto& i : caveSoundPos)
	{
		float dist = sqrt(
		(pos.x - i.x) * (pos.x - i.x) +
		(pos.y - i.y) * (pos.y - i.y)
			);

		if (shortestDist < 0)
		{
			shortestDist = dist;
		}
		else
		{
			shortestDist = std::min(shortestDist, dist);
		}
	}

	if (shortestDist < 0)
		return 0;

	return distFunc(shortestDist) * 0.5;
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
			data[i].mainColor = glm::vec4(1, 1, 1, 1);


		}
	}

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			auto& d = get(x, y);
			if (isWaterMusicSource(d.type))
			{
				waterPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
			}

			if (d.type == Block::musicGreen)
			{
				greenSoundPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
				d.type = Block::none;
			}
			else
				if (d.type == Block::musicRed)
				{
					redSoundPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
					d.type = Block::none;
				}
				else
					if (d.type == Block::musicTiki)
					{
						tikiSoundPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
						d.type = Block::none;
					}
					else
						if (d.type == Block::musicSnow)
						{
							snowSoundPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
							d.type = Block::none;
						}
						else
							if (d.type == Block::musicCave)
							{
								caveSoundPos.push_back(glm::vec2{ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
								d.type = Block::none;
							}
		}

	setNeighbors();
	//todo setup neighbours
}

BlockInfo& MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

void MapData::clearColorData()
{
	if (data)
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].mainColor = { 0,0,0,0 };
			data[i].sideColors = { 0,0,0,0 };
			data[i].resetColors();
		}
	}
}

void MapData::cleanup()
{
	waterPos.clear();
	if (data)
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

