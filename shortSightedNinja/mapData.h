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

struct Edge
{
	float sx, sy;
	float ex, ey;
};

struct BlockInfo
{
	bool hasNeighborLeft();
	bool hasNeighborRight();
	bool hasNeighborTop();
	bool hasNeighborDown();
	unsigned char neighbors;

	int edgeId[4];
	bool edgeExist[4];


	glm::vec4 mainColor;

	char type;

	void resetColors();
};

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
struct VisibilityPolygonPoints
{
	float angle;
	float x, y;
};
struct MapData
{
	BlockInfo *data;

	int w;
	int h;

	std::vector<Edge> vecEdges;

	
	std::vector<VisibilityPolygonPoints> vecVisibilityPolygonPoints;

	void create(int w, int h, const char* d);
	BlockInfo &get(int x, int y);

	void ConvertTileMapToPolyMap();

	void CalculateVisibilityPolygon(float ox, float oy, float radius);

	void clearColorData();
	
	void setNeighbors();

	void cleanup();
};

