#pragma once
#include "glm/vec4.hpp"
#include <vector>

enum Block : char
{
	none = ' ',
	dirt,
};

inline bool isAir(char b)
{
	return b == none;
}

inline bool isOpaque(char b)
{
	return b == dirt;
}

inline bool isColidable(char b)
{
	//todo
	return b == dirt;

}

struct BlockInfo
{
	bool hasNeighbourLeft();
	bool hasNeighbourRight();
	bool hasNeighbourTop();
	bool hasNeighbourDown();
	unsigned char neighBours;

	glm::vec4 mainColor;

	char type;

	void resetColors();
};

struct MapData
{

	BlockInfo *data;

	int w;
	int h;

	void create(int w, int h, const char* d);
	BlockInfo &get(int x, int y);

	void clearColorData();
	
	void createEdges();
	void setNeighbours();

	std::vector<glm::vec4> edges;

	void cleanup();
};

