#pragma once
#include "mapData.h"
#include "glm/vec2.hpp"
#include <vector>
#include "Entity.h"

void simuleteLightSpot(glm::vec2 pos, float radius, MapData & mapData, std::vector<Arrow> &arrows, 
	std::vector<Pickup> &pickups);

struct LightSource
{
	LightSource() {};
	LightSource(const glm::ivec2 &pos, float intensity):pos(pos), intensity(intensity)
	{
		animationDuration = animationStartTime;
	};

	LightSource(const glm::ivec2 &pos, float animDur, float intensity) :pos(pos),
		animationDuration(animDur), intensity(intensity)
	{};

	LightSource(const glm::ivec2 &pos, float animDur, float intensity, int w, int h) :pos(pos),
		animationDuration(animDur), intensity(intensity), boxW(w), boxH(h)
	{};

	static constexpr float animationStartTime = 2;
	glm::ivec2 pos = {};
	float animationDuration;

	//in units 1 2 3 ,, etc
	float intensity = 0;

	float boxW = 0;
	float boxH = 0;

};