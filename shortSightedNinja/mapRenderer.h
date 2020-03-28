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
		//todo add texture data
		BUFFERS_SIZE
	};

	GLuint vao;
	GLuint buffers[Buffers::BUFFERS_SIZE];

	ShaderProgram shader;

	void init(ShaderProgram s);

	void addBlock(glm::vec4 pos);

	void clearBlockData();
	void render();

	std::vector<glm::vec2> positionVector;

};

