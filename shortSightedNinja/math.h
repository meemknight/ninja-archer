#pragma once
#include "mapData.h"
#include "glm/vec2.hpp"
#include <vector>
#include "Entity.h"

void simuleteLightSpot(glm::vec2 pos, float radius, MapData & mapData, std::vector<Arrow> &arrows, 
	std::vector<Pickup> &pickups, float heat);

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

	static constexpr int animationStartTime = 2;
	glm::ivec2 pos = {};
	float animationDuration;
	float intensity = 5;
};