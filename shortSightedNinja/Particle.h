#pragma once
#include "glm/vec2.hpp"
#include "opengl2Dlib.h"

struct Particle
{
	glm::vec2 pos;

	int type;

	float duration = 0;

	static constexpr float maxDuration = 1;

	void draw(gl2d::Renderer2D & renderer2d, float deltaTime, gl2d::Texture particleSprite);

};

