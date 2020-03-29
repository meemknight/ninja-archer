#pragma once
#include "glm/vec4.hpp"

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

struct BlockInfo
{
	bool canBeLitLeft();
	bool canBeLitRight();
	bool canBeLitTop();
	bool canBeLitDown();
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

	void cleanup();
};

