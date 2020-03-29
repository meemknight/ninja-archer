#include "MapRenderer.h"

void MapRenderer::init(ShaderProgram s)
{
	positionVector.reserve(200);

	shader = s;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.bind();
	spritesUniform = glGetUniformLocation(shader.id, "u_sprites");

	glGenBuffers(BUFFERS_SIZE,buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[textureCoords]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void MapRenderer::addBlock(glm::vec4 pos, glm::vec4 texCoord)
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
}

void MapRenderer::clearBlockDrawData()
{
	positionVector.clear();
	textureCoordVector.clear();
}

void MapRenderer::render()
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glBufferData(GL_ARRAY_BUFFER, positionVector.size() * sizeof(*positionVector.data()), positionVector.data(), GL_STREAM_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[textureCoords]);
	glBufferData(GL_ARRAY_BUFFER, textureCoordVector.size() * sizeof(*textureCoordVector.data()), textureCoordVector.data(), GL_STREAM_DRAW);
	
	shader.bind();
	sprites.bind(0);
	glUniform1i(spritesUniform, 0);

	glDrawArrays(GL_TRIANGLES, 0, positionVector.size());

	glBindVertexArray(0);
	clearBlockDrawData();
}
