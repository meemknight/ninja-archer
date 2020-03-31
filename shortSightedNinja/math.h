#pragma once
#include "mapData.h"
#include "glm/vec2.hpp"
#include <vector>

void simulateLight(glm::vec2 pos, float radius, MapData& mapData, std::vector<glm::vec2>& triangles);

void simuleteLightTrace(glm::vec2 pos, float radius, MapData& mapData, std::vector<glm::vec2>& triangles);