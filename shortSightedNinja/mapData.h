#pragma once
#include "glm/vec4.hpp"
#include <vector>
#define BLOCK_SIZE 8
#define BLOCK_COUNT 424

namespace Block
{
	enum : unsigned short
	{
		none = 0,
		bareer,
		solidBrick1,
		solidBrick2,
		solidBrick3,
		solidBrick4,
		solidBrick5,
		solidBrick6,
		grass1,
		grass2,
		grass3,
		grass4,
		grass5,
		grass6,
		grass7,
		grass8,
		grass9,
		grass10,
		grass11,
		grass12,
		grass13,
		redSolid1,
		redNoSolid1,
		redSolid2,
		redNoSolid2,
		redSolid3,
		redNoSolid3,
		redSolid4,
		redNoSolid4,
		redSolid5,
		redNoSolid5,
		redSolid6,
		redNoSolid6,
		redSolid7,
		redNoSolid7,
		redSolid8,
		redNoSolid8,
		redSolidFence,
		redNoSolidFence,
		blueSolid1,
		blueNoSolid1,
		blueSolid2,
		blueNoSolid2,
		blueSolid3,
		blueNoSolid3,
		blueSolid4,
		blueNoSolid4,
		blueSolid5,
		blueNoSolid5,
		blueSolid6,
		blueNoSolid6,
		blueSolid7,
		blueNoSolid7,
		blueSolid8,
		blueNoSolid8,
		blueSolidFence,
		blueNoSolidFence,
		brickSolid1,
		brickSolid2,
		brickSolid3,
		brickSolid4,
		brickSolid5,
		brickSolid6,
		brickSolid7,
		brickSolid8,
		brickSolid9,
		brickSolid10,
		waterSolid1,
		waterSolid2,
		waterSolid3,
		waterSolid4,
		waterSolid5,
		waterSolid6,
		stoneSolid1,
		stoneSolid2,
		stoneSolid3,
		stoneSolid4,
		stoneSolid5,
		stoneSolid6,
		stoneSolid7,
		stoneSolid8,
		stoneSolid9,
		stoneSolid10, //rock
		woodSolid1,
		woodSolid2,
		woodSolid3,
		woodSolid4,
		woodSolid5,
		woodSolid6,
		woodSolid7,
		woodSolid8,
		woodSolid9,
		woodSolid10,
		woodSolid11, // crates
		fenceSolid,
		fenceNoSolid,
		snowSolid1,
		snowSolid2,
		snowSolid3,
		snowSolid4,
		snowSolid5,
		snowSolid6,
		snowSolid7,
		snowSolid8,
		snowSolid9,
		stoneSolid11,
		stoneSolid12,
		rockCracked,
		tikiTotem,
		woodSolid12,
		woodSolid13,
		woodSolid14,
		bridge1,
		bridge2,
#pragma region unfinished

		unfinished1, unfinished2, unfinished3, unfinished4, unfinished5,
		unfinished6, unfinished7, unfinished8, unfinished9, unfinished10,
		unfinished11, unfinished12, unfinished13, unfinished14, unfinished15,
		unfinished16, unfinished17, unfinished18, unfinished19, unfinished20,
		unfinished21, unfinished22, unfinished23, unfinished24, unfinished25,
		unfinished26, unfinished27, unfinished28, unfinished29, unfinished30,
		unfinished31, unfinished32, unfinished33, unfinished34, unfinished35,
		unfinished36, unfinished37, unfinished38, unfinished39, unfinished40,
		unfinished41, unfinished42, unfinished43, unfinished44, unfinished45,
		unfinished46, unfinished47, unfinished48, unfinished49, unfinished50,
		unfinished51, unfinished52, unfinished53, unfinished54, unfinished55,
		unfinished56, unfinished57, unfinished58, unfinished59, unfinished60,
		unfinished61, unfinished62, unfinished63, unfinished64, unfinished65,
		unfinished66, unfinished67, unfinished68, unfinished69, unfinished70,
		unfinished71, unfinished72, unfinished73, unfinished74, unfinished75,
#pragma endregion

		chainDecoration, /////////////////////////////start walls
		brickDecoration1,
		brickDecoration2,
		brickDecoration3,
		brickDecoration4,
		brickDecoration5,
		grassDecoration,
		leavesRight,
		leavesLeft,
		vines1,
		vines2,
		torceBottom,
		torceTopBrickUnlit,
		torceTopBrickLit,
		torceTopLeavesUnlit,
		torceTopLeavesLit,
		leavesDecoration1,
		leavesDecoration2,
		leavesDecoration3,
		leavesDecoration4,
		windowDecoration,
		trunk1,
		trunk2,
		sign,
		waterFall,
		waterFallBegin,
		waterFallEnd,
		water1,
		water2,
		water3,
		water4,
		friendlyWater,
		water5,
		water6,
		water7,
		water8,
		water9,
		water10,
		targetRed,
		targetBlue,
		woodDecoration1,
		woodDecoration2,
		targetKey,
		snowDecoration1,
		snowDecoration2,
		grassDecoration2,
		grassDecoration3,
		unlitLantern,
		litLantern,
		webDecoration1,
		webDecoration2,
		signDecoration1,
		signDecoration2,
		signDecoration3,
		grassDecoration4,
		flagDown,
		flagUp,
		woddenArrow,
		fireArrow,
		slimeArrow,
		keyArrow,
		levelExit,
		webDecoration3,
		signDecoration4,
		signDecoration5,
		webBlock,
		tikiDecoration1,
		tikiDecoration2,
		tikiDecoration3,
		skullDecoration,
		bombArrow,
		woddenDecoration1,
		woddenDecoration2,
		woddenDecoration3,
		woddenDecoration4,
		woddenDecoration5,
		woddenDecoration6,
		woddenDecoration7,
		musicGreen,
		musicRed,
		musicTiki,
		musicSnow,
		musicCave,
		leavesDecoration,
		tikiPole,
		torchUnlitWood,
		torchLitWood,
		treeDecoration1,
		treeDecoration2,
		treeDecoration3,
		treeDecoration4,
		treeDecoration5,
		treeDecoration6,
		treeDecoration7,
		treeDecoration8,
		treeDecoration9,
		treeDecoration10,
		treeDecoration11,
		treeDecoration12,
		treeDecoration13,
		treeDecoration14,
		treeDecoration15,
		treeDecoration16,
		treeDecoration17,
		treeDecoration18,
		webDecoration4,
		webDecoration5,
		lastBlock,
	};
};

inline bool isUnfinished(unsigned short b)
{
	return b >= Block::unfinished1 &&b <= Block::unfinished75;
}


inline bool isAir(unsigned short b)
{
	return b == Block::none;
}

inline bool isRedSolid(unsigned short b)
{
	if (
		b == Block::redSolid1 ||
		b == Block::redSolid2 ||
		b == Block::redSolid3 ||
		b == Block::redSolid4 ||
		b == Block::redSolid5 ||
		b == Block::redSolid6 ||
		b == Block::redSolid7 ||
		b == Block::redSolid8 ||
		b == Block::redSolidFence
		)
	{
		return 1;
	}
	else { return 0; }
}

inline bool isRedNoSolid(unsigned short b)
{
	if (
		b == Block::redNoSolid1 ||
		b == Block::redNoSolid2 ||
		b == Block::redNoSolid3 ||
		b == Block::redNoSolid4 ||
		b == Block::redNoSolid5 ||
		b == Block::redNoSolid6 ||
		b == Block::redNoSolid7 ||
		b == Block::redNoSolid8 ||
		b == Block::redNoSolidFence
		)
	{
		return 1;
	}
	else { return 0; }
}


inline bool isBlueSolid(unsigned short b)
{
	if (
		b == Block::blueSolid1 ||
		b == Block::blueSolid2 ||
		b == Block::blueSolid3 ||
		b == Block::blueSolid4 ||
		b == Block::blueSolid5 ||
		b == Block::blueSolid6 ||
		b == Block::blueSolid7 ||
		b == Block::blueSolid8 ||
		b == Block::blueSolidFence
		)
	{
		return 1;
	}
	else { return 0; }
}

inline bool isBlueNoSolid(unsigned short b)
{
	if (
		b == Block::blueNoSolid1 ||
		b == Block::blueNoSolid2 ||
		b == Block::blueNoSolid3 ||
		b == Block::blueNoSolid4 ||
		b == Block::blueNoSolid5 ||
		b == Block::blueNoSolid6 ||
		b == Block::blueNoSolid7 ||
		b == Block::blueNoSolid8 ||
		b == Block::blueNoSolidFence
		)
	{
		return 1;
	}
	else { return 0; }
}

inline bool isColidable(unsigned short b)
{

	if (
		b == Block::none ||
		b == Block::redNoSolid1 ||
		b == Block::redNoSolid2 ||
		b == Block::redNoSolid3 ||
		b == Block::redNoSolid4 ||
		b == Block::redNoSolid5 ||
		b == Block::redNoSolid6 ||
		b == Block::redNoSolid7 ||
		b == Block::redNoSolid8 ||
		b == Block::redNoSolidFence ||
		b == Block::blueNoSolid1 ||
		b == Block::blueNoSolid2 ||
		b == Block::blueNoSolid3 ||
		b == Block::blueNoSolid4 ||
		b == Block::blueNoSolid5 ||
		b == Block::blueNoSolid6 ||
		b == Block::blueNoSolid7 ||
		b == Block::blueNoSolid8 ||
		b == Block::blueNoSolidFence ||
		b == Block::fenceNoSolid ||
		b == Block::snowSolid2 ||
		b == Block::snowSolid9||
		b == Block::bridge2
		) {
		return 0;
	}

	if (b < Block::chainDecoration)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

inline bool isOpaque(unsigned short b)
{
	return isColidable(b);
}

inline bool isLitTorch(unsigned short b)
{
	return b == Block::torceTopBrickLit || b == Block::torceTopLeavesLit || b == Block::litLantern
		|| b == Block::torchLitWood;
}

inline bool unLitTorch(unsigned short  b)
{
	return b == Block::torceTopBrickUnlit || b == Block::torceTopLeavesUnlit || b == Block::unlitLantern
		|| b == Block::torchUnlitWood;

}

inline bool isWaterMusicSource(unsigned short b)
{
	if (
		b == Block::waterFallBegin ||
		b == Block::waterFallEnd ||
		b == Block::water7 ||
		b == Block::water4
		)
	{
		return true;
	}
	else
	{
		return 0;
	}
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

	float heat;

	glm::vec4 mainColor;
	glm::vec4 sideColors;

	bool startAnim = 0;
	signed char animPos = 0;
	float timePassed = 0;
	unsigned short type;
	bool playerEntered = 0;
	bool playerLeft = 0;
	bool leftAnim = 1;

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
	BlockInfo* data;

	int w;
	int h;

	std::vector<Edge> vecEdges;

	std::vector<VisibilityPolygonPoints> vecVisibilityPolygonPoints;

	void create(int w, int h, const unsigned short* d);
	BlockInfo& get(int x, int y);


	void CalculateVisibilityPolygon(float ox, float oy, float radius);

	void clearColorData();

	void setNeighbors();

	void cleanup();
};

