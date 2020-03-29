#pragma once
#include <vector>
#include <glm/vec4.hpp>
#include <GL/glew.h>
#include "opengl2Dlib.h"
#include "Shader.h"
#include "mapData.h"

#define BLOCK_SIZE 16
#define BLOCK_COUNT 16

struct MapRenderer
{

	enum Buffers
	{
		blockPositionData=0,
		textureCoords,
		mainColor,
		//todo add texture data
		BUFFERS_SIZE
	};

	GLuint vao;

	GLuint buffers[Buffers::BUFFERS_SIZE];

	ShaderProgram shader;

	void init(ShaderProgram s);

	void addBlock(glm::vec4 pos, glm::vec4 texCoord, const glm::vec4 &color);

	void clearBlockDrawData();
	void render();

	//6 positions
	std::vector<glm::vec2> positionVector;
	std::vector<glm::vec2> textureCoordVector;
	std::vector<glm::vec4> mainColorVector;

	gl2d::Texture sprites;
	
	void drawFromMapData(gl2d::Renderer2D &renderer ,MapData &mapData);

	GLint spritesUniform;
};

