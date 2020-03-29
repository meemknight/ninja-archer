#include "MapRenderer.h"

void MapRenderer::init(ShaderProgram s)
{
	positionVector.reserve(200);

	shader = s;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	s.bind();

	glGenBuffers(BUFFERS_SIZE,buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(0);

}

void MapRenderer::addBlock(glm::vec4 pos)
{
	positionVector.emplace_back(glm::vec2{ pos.x,pos.y }); //top left
	positionVector.emplace_back(glm::vec2{ pos.x,pos.w }); //bottom left
	positionVector.emplace_back(glm::vec2{ pos.z,pos.y }); //top right
	positionVector.emplace_back(glm::vec2{ pos.z,pos.y }); //top right
	positionVector.emplace_back(glm::vec2{ pos.x,pos.w }); //bottom left
	positionVector.emplace_back(glm::vec2{ pos.z,pos.w }); //bottom right
}

void MapRenderer::clearBlockData()
{
	positionVector.clear();
}

void MapRenderer::render()
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[blockPositionData]);
	glBufferData(GL_ARRAY_BUFFER, positionVector.size() * sizeof(*positionVector.data()), positionVector.data(), GL_STREAM_DRAW);
	shader.bind();

	glDrawArrays(GL_TRIANGLES, 0, positionVector.size());

	glBindVertexArray(0);
	clearBlockData();
}
