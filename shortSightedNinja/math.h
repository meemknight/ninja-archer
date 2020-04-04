#pragma once
#include "mapData.h"
#include "glm/vec2.hpp"
#include <vector>
#include "Entity.h"

void simulateLight(glm::vec2 pos, float radius, MapData& mapData, std::vector<glm::vec2>& triangles);

void simuleteLightTrace(glm::vec2 pos, float radius, MapData& mapData, std::vector<glm::vec2>& triangles);

void simuleteLightSpot(glm::vec2 pos, float radius, MapData& mapData, std::vector<Arrow> &arrows, gl2d::Renderer2D &maskRenderer, gl2d::Texture lightT, float heat);