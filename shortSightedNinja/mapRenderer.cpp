#include "MapRenderer.h"

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

void MapRenderer::drawFromMapData(gl2d::Renderer2D &renderer, MapData & mapData)
{
	for(int h=0; h<mapData.h; h++)
	{
		for(int w=0;w<mapData.w; w++)
		{
			
			if(mapData.get(w, h).type != Block::none && mapData.get(w,h).mainColor.w != 0)
			{
				glm::vec4 sideC = mapData.get(w, h).sideColors;

				if (mapData.get(w, h).hasNeighborTop()) { sideC.x = 0; }
				if (mapData.get(w, h).hasNeighborDown()) { sideC.y = 0; }
				if (mapData.get(w, h).hasNeighborLeft()) { sideC.z = 0; }
				if (mapData.get(w, h).hasNeighborRight()) { sideC.w = 0; }

				addBlock(renderer.toScreen({ w*BLOCK_SIZE,h*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE }), 
					spriteAtlas.get(mapData.get(w, h).type- Block::none-1,0)
					, mapData.get(w, h).mainColor, sideC);

			}

		}
	
	}

	this->render();

	this->clearBlockDrawData();
}
