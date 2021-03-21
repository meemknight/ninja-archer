#pragma once
#include "glm/vec2.hpp"
#include "gl2d.h"

struct Particle
{
	int frameCount = 4;
	int animCount = 1;

	glm::vec2 pos;

	int type;



	float duration = 0;
	int lookingRight = 0;

	float maxDuration = 0.09;

	void set(glm::vec2 p, int type, int reverse);

	void draw(gl2d::Renderer2D & renderer2d, float deltaTime, gl2d::Texture particleSprite);

};

