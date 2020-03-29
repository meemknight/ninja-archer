#pragma once
#include <vector>
#include <glm/vec4.hpp>
#include <GL/glew.h>
#include "opengl2Dlib.h"
#include "Shader.h"

struct MapRenderer
{

	enum Buffers
	{
		blockPositionData=0,
		textureCoords,
		//todo add texture data
		BUFFERS_SIZE
	};

	GLuint vao;

	GLuint buffers[Buffers::BUFFERS_SIZE];

	ShaderProgram shader;

	void init(ShaderProgram s);

	void addBlock(glm::vec4 pos, glm::vec4 texCoord);

	void clearBlockDrawData();
	void render();

	//6 positions
	std::vector<glm::vec2> positionVector;
	std::vector<glm::vec2> textureCoordVector;

	gl2d::Texture sprites;
	
	GLint spritesUniform;
};

