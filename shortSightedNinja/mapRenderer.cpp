#include "MapRenderer.h"
#include <algorithm>
#undef min
#undef max

gl2d::TextureAtlas spriteAtlas(BLOCK_COUNT, 4);

void MapRenderer::init(ShaderProgram s)
{
	positionVector.reserve(200);
	textureCoordVector.reserve(200);
	mainColorVector.reserve(200);
	sideColors.reserve(200);
	textureLightCoordVector.reserve(200);

	shader = s;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.bind();
	spritesUniform = glGetUniformLocation(shader.id, "u_sprites");
	upUniform = glGetUniformLocation(shader.id, "u_up");
	downUniform = glGetUniformLocation(shader.id, "u_down");
	leftUniform = glGetUniformLocation(shader.id, "u_left");
	rightUniform = glGetUniformLocation(shader.id, "u_right");

	glGenBuffers(BUFFERS_SIZE,buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[textureCoords]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[mainColor]);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::sideColorsBuf]);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::textureColorBuf]);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);

}

void MapRenderer::addBlock(const glm::vec4 &pos, const glm::vec4 &texCoord, const glm::vec4 &color,const glm::vec4 &sideLuminosity)
{
	positionVector.emplace_back(glm::vec2{ pos.x,pos.y }); //top left
	positionVector.emplace_back(glm::vec2{ pos.x,pos.w }); //bottom left
	positionVector.emplace_back(glm::vec2{ pos.z,pos.y }); //top right
	positionVector.emplace_back(glm::vec2{ pos.z,pos.y }); //top right
	positionVector.emplace_back(glm::vec2{ pos.x,pos.w }); //bottom left
	positionVector.emplace_back(glm::vec2{ pos.z,pos.w }); //bottom right

	textureCoordVector.emplace_back(glm::vec2{ texCoord.x,texCoord.y }); //top left
	textureCoordVector.emplace_back(glm::vec2{ texCoord.x,texCoord.w }); //bottom left
	textureCoordVector.emplace_back(glm::vec2{ texCoord.z,texCoord.y }); //top right
	textureCoordVector.emplace_back(glm::vec2{ texCoord.z,texCoord.y }); //top right
	textureCoordVector.emplace_back(glm::vec2{ texCoord.x,texCoord.w }); //bottom left
	textureCoordVector.emplace_back(glm::vec2{ texCoord.z,texCoord.w }); //bottom right

	mainColorVector.emplace_back(color); //top left
	mainColorVector.emplace_back(color); //bottom left
	mainColorVector.emplace_back(color); //top right
	mainColorVector.emplace_back(color); //top right
	mainColorVector.emplace_back(color); //bottom left
	mainColorVector.emplace_back(color); //bottom right

	sideColors.emplace_back(sideLuminosity);
	sideColors.emplace_back(sideLuminosity);
	sideColors.emplace_back(sideLuminosity);
	sideColors.emplace_back(sideLuminosity);
	sideColors.emplace_back(sideLuminosity);
	sideColors.emplace_back(sideLuminosity);

	textureLightCoordVector.emplace_back(glm::vec2{0,1}); //top left
	textureLightCoordVector.emplace_back(glm::vec2{0,0}); //bottom left
	textureLightCoordVector.emplace_back(glm::vec2{1,1}); //top right
	textureLightCoordVector.emplace_back(glm::vec2{1,1}); //top right
	textureLightCoordVector.emplace_back(glm::vec2{0,0}); //bottom left
	textureLightCoordVector.emplace_back(glm::vec2{1,0}); //bottom right
}

void MapRenderer::clearBlockDrawData()
{
	positionVector.clear();
	textureCoordVector.clear();
	mainColorVector.clear();
	sideColors.clear();
	textureLightCoordVector.clear();
}

void MapRenderer::render()
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glBufferData(GL_ARRAY_BUFFER, positionVector.size() * sizeof(*positionVector.data()), positionVector.data(), GL_STREAM_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[textureCoords]);
	glBufferData(GL_ARRAY_BUFFER, textureCoordVector.size() * sizeof(*textureCoordVector.data()), textureCoordVector.data(), GL_STREAM_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[mainColor]);
	glBufferData(GL_ARRAY_BUFFER, mainColorVector.size() * sizeof(*mainColorVector.data()), mainColorVector.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::sideColorsBuf]);
	glBufferData(GL_ARRAY_BUFFER, sideColors.size() * sizeof(*sideColors.data()), sideColors.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::textureColorBuf]);
	glBufferData(GL_ARRAY_BUFFER, textureLightCoordVector.size() * sizeof(*textureLightCoordVector.data()), textureLightCoordVector.data(), GL_STREAM_DRAW);

	shader.bind();
	sprites.bind(0);
	upTexture.bind(1);
	downTexture.bind(2);
	leftTexture.bind(3);
	rightTexture.bind(4);

	glUniform1i(spritesUniform, 0);
	glUniform1i(upUniform, 1);
	glUniform1i(downUniform, 2);
	glUniform1i(leftUniform, 3);
	glUniform1i(rightUniform, 4);

	glDrawArrays(GL_TRIANGLES, 0, positionVector.size());

	glBindVertexArray(0);
	clearBlockDrawData();
}

float animDuration = 0.12;

void MapRenderer::drawFromMapData(gl2d::Renderer2D &renderer, MapData & mapData, float deltaTime, int curPos)
{
	
	glm::vec2 minPos = { 0,0 };
	glm::vec2 maxPos = {0,0};

	minPos.x = ((renderer.currentCamera.position.x));
	minPos.y = ((renderer.currentCamera.position.y));
	maxPos.x = ((renderer.currentCamera.position.x+renderer.windowW));
	maxPos.y = ((renderer.currentCamera.position.y+renderer.windowH));

	minPos = gl2d::scaleAroundPoint(minPos, renderer.currentCamera.position +
		glm::vec2{ renderer.windowW / 2, renderer.windowH / 2 }, 1.f/renderer.currentCamera.zoom);

	maxPos= gl2d::scaleAroundPoint(maxPos, renderer.currentCamera.position +
		glm::vec2{ renderer.windowW / 2, renderer.windowH / 2 }, 1.f/renderer.currentCamera.zoom);

	minPos /= BLOCK_SIZE;
	maxPos /= BLOCK_SIZE;

	minPos.x--;
	minPos.y--;

	maxPos.x++;
	maxPos.y++;

	minPos = glm::max(minPos, glm::vec2{ 0,0 });
	maxPos = glm::min(maxPos, glm::vec2{ mapData.w,mapData.h });

	for(int h= minPos.y; h<maxPos.y; h++)
	{
		for(int w= minPos.x;w<maxPos.x; w++)
		{
			auto &g = mapData.get(w, h);
			if(g.type != Block::none && g.mainColor.w != 0)
			{
				glm::vec4 sideC = g.sideColors;

				if(!isColidable(g.type))
				{
					sideC = {};
				}else
				{
					if (g.hasNeighborTop()) { sideC.x = 0; }
					if (g.hasNeighborDown()) { sideC.y = 0; }
					if (g.hasNeighborLeft()) { sideC.z = 0; }
					if (g.hasNeighborRight()) { sideC.w = 0; }
				}

				auto color = g.mainColor;

				//color.g *= g.heat;
				//color.b *= g.heat;

				color = { 1,1,1,color.r };

				auto &data = mapData.get(w, h);
				
				//always
				if (data.type == Block::waterFallBegin || data.type == Block::waterFallEnd
					|| data.type == Block::torceTopBrickLit
					|| data.type == Block::torceTopLeavesLit
					|| data.type == Block::litLantern
					|| data.type == Block::water4
					|| data.type == Block::water5
					|| data.type == Block::water6
					|| data.type == Block::water7
					|| data.type == Block::water9
					|| data.type == Block::water10
					)
				{
					data.animPos = curPos;
				}
				else if (//player
					data.type == Block::grassDecoration ||
					data.type == Block::leavesRight ||
					data.type == Block::leavesLeft ||
					data.type == Block::vines1||
					data.type == Block::vines2 ||
					data.type == Block::snowDecoration1||
					data.type == Block::waterFall ||
					data.type == Block::grassDecoration2||
					data.type == Block::grassDecoration3 ||
					data.type == Block::water3||
					data.type == Block::water1 ||
					data.type == Block::snowSolid2 ||
					data.type == Block::snowSolid9 ||
					data.type == Block::snowDecoration2||
					data.type == Block::grassDecoration4 ||
					data.type == Block::webDecoration1 ||
					data.type == Block::webDecoration2 ||
					data.type == Block::friendlyWater
					
					)
				{

					if(data.playerEntered)
					{
						if(data.playerLeft)
						{
							data.leftAnim = 0;
							data.startAnim = 1;
						}
						data.playerLeft = 0;
					}else
					{
						data.playerLeft = 1;
						if(!data.leftAnim)
						{
							data.leftAnim = 1;
							data.startAnim = 1;
						}
					}

					if(data.startAnim == 1)
					{
						data.timePassed += deltaTime;
						while(data.timePassed > animDuration)
						{
							data.timePassed -= animDuration;
							data.animPos++;
						}
						if(data.animPos >3)
						{
							data.startAnim = 0;
							data.animPos = 0;
						}
					}
				}

				data.playerEntered = 0;

				addBlock(renderer.toScreen({ w*BLOCK_SIZE,h*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE }), 
					spriteAtlas.get(data.type- Block::none-1, data.animPos)
					,color , sideC);

			}

		}
	
	}

	this->render();

	this->clearBlockDrawData();
}
