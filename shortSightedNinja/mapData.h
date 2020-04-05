#pragma once
#include "glm/vec4.hpp"
#include <vector>

enum Block : char
{
	none = ' ',
	dirt,
	grassLeft,
	gras,
	grasRight,
	stone,
	brick,
	redTarget,
	blueTarget,
	greenTarget,
	redBlock,
	redNo,
	blueBlock,
	blueNo,
	greenBlock,
	greenNo,
	yellowBlock,
	yellowNo,
	keyHole,
	dirtBackground,
	stoneBackground,
	dirtTorch,
	dirtLitTorch,
	stoneTorch,
	stoneLitTorch,
	brickTorch,
	brickLitTorch,
	brickBackground,
	unlitTorch,
	litTorch,
	pole,
	lastBlock,
};

inline bool isAir(char b)
{
	return b == none;
}

inline bool isColidable(char b)
{
	
	if(b == redNo ||
		b == blueNo ||
		b == greenNo ||
		b == yellowNo||
		b == dirtBackground ||
		b == stoneBackground ||
		b == dirtTorch ||
		b == stoneTorch ||
		b == stoneLitTorch ||
		b == dirtLitTorch ||
		b == brickLitTorch ||
		b == brickTorch ||
		b == brickBackground ||
		b == litTorch ||
		b == unlitTorch ||
		b == pole
		) {
		return 0;
	}

	return b != none;

}

inline bool isOpaque(char b)
{
	return isColidable(b);
}

inline bool isLitTorch(char b)
{
	return b == dirtLitTorch | b == brickLitTorch | b == stoneLitTorch | b==litTorch;
}

inline bool unLitTorch(char b)
{
	return b == dirtTorch | b == brickTorch | b == stoneTorch | b == unlitTorch;
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

	//up down left right 0000 udlr;
	unsigned char neighbors;

	int edgeId[4];
	bool edgeExist[4];

	//top down left right
	glm::vec4 directionalLight;

	float alpha;
	float heat;

	glm::vec4 mainColor;
	glm::vec4 sideColors;

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

