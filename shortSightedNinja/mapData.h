#pragma once
struct MapData
{
	enum class Block : unsigned char
	{
		none,
		dirt,
	};

	Block *data;

	int w;
	int h;

	void create(int w, int h, void* d);
	Block &get(int x, int y);

	void cleanup();
};

