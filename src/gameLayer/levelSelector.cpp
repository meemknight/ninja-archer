#include "levelSelector.h"
#include "menu.h"
#include "mapData.h"
#include "platformInput.h"
#include "input.h"
#include "Ui.h"
#include "Shader.h"
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "OBJ_Loader.h"

const char *levelNames[LEVELS] = { "Tutorial", "Enchanted forest", "Cave", "Tiki tribe", "Secret Level", "test world", "shire" };


ShaderProgram shader;
GLuint vao;
GLuint buffer;
GLuint indexBuffer;
GLint u_ModelViewProjection = -1;
GLint u_modelToWorld = -1;
GLint u_eyePosition = -1;
gl2d::Texture colorTexture;
GLint u_albedo = -1;

int primitiveCount;

float uv = 1;
float cubePositionsNormals[] = {
	-1.0f, +1.0f, +1.0f, // 0
	+0.0f, +1.0f, +0.0f, // Normal
	0, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, +1.0f, +1.0f, // 1
	+0.0f, +1.0f, +0.0f, // Normal
	1 * uv, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, +1.0f, -1.0f, // 2
	+0.0f, +1.0f, +0.0f, // Normal
	1 * uv, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, +1.0f, -1.0f, // 3
	+0.0f, +1.0f, +0.0f, // Normal
	0, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent



	-1.0f, +1.0f, -1.0f, // 4
	 0.0f, +0.0f, -1.0f, // Normal
	 0, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, +1.0f, -1.0f, // 5
	 0.0f, +0.0f, -1.0f, // Normal
	 1 * uv, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	 +1.0f, -1.0f, -1.0f, // 6
	 0.0f, +0.0f, -1.0f, // Normal
	 1 * uv, 0,				 //uv
	 //0,0,0,				 //tangent
	 //0,0,0,				 //btangent

	-1.0f, -1.0f, -1.0f, // 7
	 0.0f, +0.0f, -1.0f, // Normal
	 0, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, +1.0f, -1.0f, // 8
	+1.0f, +0.0f, +0.0f, // Normal
	1 * uv, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, +1.0f, +1.0f, // 9
	+1.0f, +0.0f, +0.0f, // Normal
	1 * uv, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, -1.0f, +1.0f, // 10
	+1.0f, +0.0f, +0.0f, // Normal
	0, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	+1.0f, -1.0f, -1.0f, // 11
	+1.0f, +0.0f, +0.0f, // Normal
	0, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, +1.0f, +1.0f, // 12
	-1.0f, +0.0f, +0.0f, // Normal
	1 * uv, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, +1.0f, -1.0f, // 13
	-1.0f, +0.0f, +0.0f, // Normal
	1 * uv, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, -1.0f, -1.0f, // 14
	-1.0f, +0.0f, +0.0f, // Normal
	0, 0,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, -1.0f, +1.0f, // 15
	-1.0f, +0.0f, +0.0f, // Normal
	0, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent


	+1.0f, +1.0f, +1.0f, // 16
	+0.0f, +0.0f, +1.0f, // Normal
	1 * uv, 1 * uv,				 //uv
	//0,0,0,				 //tangent
	//0,0,0,				 //btangent

	-1.0f, +1.0f, +1.0f, // 17
	+0.0f, +0.0f, +1.0f, // Normal
	0, 1 * uv,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

	-1.0f, -1.0f, +1.0f, // 18
	+0.0f, +0.0f, +1.0f, // Normal
	0, 0,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

	+1.0f, -1.0f, +1.0f, // 19
	+0.0f, +0.0f, +1.0f, // Normal
	1 * uv, 0,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent


	+1.0f, -1.0f, -1.0f, // 20
	+0.0f, -1.0f, +0.0f, // Normal
	1 * uv, 0,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

	-1.0f, -1.0f, -1.0f, // 21
	+0.0f, -1.0f, +0.0f, // Normal
	0, 0,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

	-1.0f, -1.0f, +1.0f, // 22
	+0.0f, -1.0f, +0.0f, // Normal
	0, 1 * uv,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

	+1.0f, -1.0f, +1.0f, // 23
	+0.0f, -1.0f, +0.0f, // Normal
	1 * uv, 1 * uv,				 //uv
	//0, 0, 0,				 //tangent
	//0, 0, 0,				 //btangent

};

unsigned int cubeIndices[] = {
0,   1,  2,  0,  2,  3, // Top
4,   5,  6,  4,  6,  7, // Back
8,   9, 10,  8, 10, 11, // Right
12, 13, 14, 12, 14, 15, // Left
16, 17, 18, 16, 18, 19, // Front
20, 22, 21, 20, 23, 22, // Bottom
};

GLint getUniform(GLuint id, const char *name)
{
	GLint uniform = glGetUniformLocation(id, name);
	if (uniform == -1)
	{
		std::cout << "uniform error " << name << "\n";
	}
	return uniform;
};

//todo get optimized version when ready
glm::mat4 getTransformMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	auto s = glm::scale(scale);
	auto r = glm::rotate(rotation.x, glm::vec3(1, 0, 0)) *
		glm::rotate(rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(rotation.z, glm::vec3(0, 0, 1));
	auto t = glm::translate(position);

	return t * r * s;
}

objl::Loader model;

void initLevelSelectorData()
{
	shader.fs = FragmentShader(RESOURCES_PATH "model.frag");
	shader.vs = VertexShader(RESOURCES_PATH "model.vert");
	shader.compileProgram();
	
	u_ModelViewProjection = getUniform(shader.id, "u_ModelViewProjection");
	u_modelToWorld = getUniform(shader.id, "u_modelToWorld");
	u_eyePosition = getUniform(shader.id, "u_eyePosition");
	u_albedo = getUniform(shader.id, "u_albedo");

	colorTexture.loadFromFile(RESOURCES_PATH "colors.png");
	//colorTexture.loadFromFile(RESOURCES_PATH "ui/art.png");

	model.LoadFile(RESOURCES_PATH "models/level1.obj");

	std::vector<float> modelData;
	modelData.reserve(model.LoadedVertices.size() * 8);

	for(auto &i :model.LoadedVertices)
	{
		modelData.push_back(i.Position.X);
		modelData.push_back(i.Position.Y);
		modelData.push_back(i.Position.Z);

		modelData.push_back(i.Normal.X);
		modelData.push_back(i.Normal.Y);
		modelData.push_back(i.Normal.Z);

		modelData.push_back(i.TextureCoordinate.X);
		modelData.push_back(i.TextureCoordinate.Y);

	}

	std::vector<unsigned int> modelIndices;

	for(auto &i :model.LoadedIndices)
	{
		modelIndices.push_back(i);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, modelData.size() * 8, modelData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelIndices.size() * 8, modelIndices.data(), GL_STATIC_DRAW);

	primitiveCount = modelIndices.size();

}

int currentLevelLooking = 0;
float rotation = 0;

glm::vec3 eyePosition = glm::vec3{ 0,1.2,1.5 };
void renderModel(float aspectRatio)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glBindVertexArray(vao);
	shader.bind();

	auto modelMatrix = getTransformMatrix({ 0,0, 0 }, {0, -rotation, 0}, { 0.6,0.6,0.6 });
	auto viewMatrix = glm::lookAt(eyePosition, {0, 0, 0}, { 0,1,0 });
	auto projectionMatrix = glm::perspective(glm::radians(100.f), aspectRatio, 0.01f, 100.f);

	glUniformMatrix4fv(u_ModelViewProjection, 1, GL_FALSE,
		&(projectionMatrix * viewMatrix * modelMatrix)[0][0]);

	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE,
		&(modelMatrix)[0][0]);

	glUniform3f(u_eyePosition, eyePosition.x, eyePosition.y, eyePosition.z);
	
	glUniform1i(u_albedo, 0);
	colorTexture.bind();

	glDrawElements(GL_TRIANGLES, primitiveCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

}

int levelSelectorMenu(float deltaTime, gl2d::Renderer2D &renderer2d, gl2d::Texture &uiDialogBox, gl2d::Font &font)
{
#pragma region input and colors

	const glm::vec4 textColor = Colors::white;
	const glm::vec4 selectedButtonColor = Colors::lightGreen;
	const glm::vec4 selectedArrowColor = Colors::darkGreen;
	const glm::vec4 uninteractebleTextColor = Colors::darkGray;
	const glm::vec4 dropShadowColor = { 0.1f,0.1f,0.1f,1.f };

	bool usingControllerInput = input::isControllerInput();
	bool acceptKeyReleased = input::isKeyReleased(input::Buttons::jump) || platform::isKeyReleased(platform::Button::Enter);
	bool leftPressed = input::isKeyPressedOn(input::Buttons::left);
	bool rightPressed = input::isKeyPressedOn(input::Buttons::right);
	bool upReleased = input::isKeyReleased(input::Buttons::up);
	bool downReleased = input::isKeyReleased(input::Buttons::down);
	//bool escReleased = input::isKeyReleased(input::Buttons::esc);

#pragma endregion

#pragma region handle input

	if (leftPressed)
	{
		currentLevelLooking--;
	}

	if (rightPressed)
	{
		currentLevelLooking++;
	}

	while(currentLevelLooking < 0)
	{
		currentLevelLooking = LEVELS + currentLevelLooking;
	}

	currentLevelLooking = currentLevelLooking % LEVELS;


	if(acceptKeyReleased)
	{
		return currentLevelLooking;
	}

#pragma endregion

#pragma region animate stuff

	rotation += deltaTime * glm::radians(35.f);

	if(rotation >= glm::radians(360.f))
	{
		rotation -= glm::radians(360.f);
	}

#pragma endregion


	renderModel((float)renderer2d.windowW / renderer2d.windowH);

	{
		Ui::Frame fullScreenFrame({ 0,0, renderer2d.windowW, renderer2d.windowH });

		auto textSize = renderer2d.getTextSize(levelNames[currentLevelLooking], font);

		auto textBox = Ui::Box().xCenter().yBottom(-textSize.y * 1.5)();

		renderer2d.renderText(glm::vec2(textBox), levelNames[currentLevelLooking], font, textColor);

	}

	return -1;

}


