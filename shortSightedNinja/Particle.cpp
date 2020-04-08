#include "Particle.h"
#include "mapData.h"

void Particle::draw(gl2d::Renderer2D & renderer2d, float deltaTime, gl2d::Texture particleSprite)
{
	auto a = particleSprite.GetSize();
	gl2d::TextureAtlasPadding ta(4, 2, a.x, a.y);

	renderer2d.renderRectangle({ pos, BLOCK_SIZE, BLOCK_SIZE }, {}, 0, particleSprite, ta.get(pos.x, pos.y));
}
