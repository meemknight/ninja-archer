#include "Particle.h"
#include "mapData.h"

void Particle::set(glm::vec2 p, int type, int reverse)
{
	pos = p;
	duration = maxDuration;
	this->type = type;
	lookingRight = reverse;
}

void Particle::draw(gl2d::Renderer2D & renderer2d, float deltaTime, gl2d::Texture particleSprite)
{
	if(duration > 0)
	{
		int frame;
		float t = maxDuration - duration;
		float perc = t / maxDuration;

		frame = perc * frameCount;

		auto a = particleSprite.GetSize();
		gl2d::TextureAtlasPadding ta(frameCount, animCount, a.x, a.y);

		renderer2d.renderRectangle({ pos, BLOCK_SIZE, BLOCK_SIZE }, {}, 0, particleSprite, ta.get(frame, type, !lookingRight));

		duration -= deltaTime;
	}


}
