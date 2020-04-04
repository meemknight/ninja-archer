#pragma once
#include <vector>
#include <glm/vec4.hpp>
#include <GL/glew.h>
#include "opengl2Dlib.h"
#include "Shader.h"
#include "mapData.h"

#define BLOCK_SIZE 16
#define BLOCK_COUNT 32

struct MapRenderer
{

	enum Buffers
	{
		blockPositionData=0,
		textureCoords,
		mainColor,
		sideColorsBuf,
		textureColorBuf,
		//todo add texture data
		BUFFERS_SIZE
	};

	GLuint vao;

	GLuint buffers[Buffers::BUFFERS_SIZE];

	ShaderProgram shader;

	void init(ShaderProgram s);

	void addBlock(const glm::vec4 &pos, const glm::vec4 &texCoord, const glm::vec4 &color,
		const glm::vec4 &sideLuminosity);

	void clearBlockDrawData();
	void render();

	//6 positions
	std::vector<glm::vec2> positionVector;
	std::vector<glm::vec2> textureCoordVector;
	std::vector<glm::vec4> mainColorVector;
	std::vector<glm::vec4> sideColors;
	std::vector<glm::vec2> textureLightCoordVector;

	gl2d::Texture sprites;
	gl2d::Texture upTexture;
	gl2d::Texture downTexture;
	gl2d::Texture leftTexture;
	gl2d::Texture rightTexture;
	
	void drawFromMapData(gl2d::Renderer2D &renderer ,MapData &mapData);

	GLint spritesUniform;
	GLint upUniform;
	GLint downUniform;
	GLint leftUniform;
	GLint rightUniform;
};

