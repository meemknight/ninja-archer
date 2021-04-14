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

#undef max

const char *levelNames[LEVELS] = { "Tutorial", "home", "Enchanted forest", "Cave", "Tiki tribe", "Ice mountain", "testWorld" };
const char *modelNames[LEVELS] = { "level1.obj",  "level2.obj","enchantedForest.obj", "cave.obj", "tikiTribe.obj", "iceLevel.obj", "level2.obj" };


ShaderProgram shader;

GLint u_ModelViewProjection = -1;
GLint u_modelToWorld = -1;
GLint u_eyePosition = -1;
GLint u_lightPos = -1;
GLint u_lightCount = -1;
gl2d::Texture colorTexture;
GLint u_albedo = -1;
GLint u_color = -1;



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


struct Model
{
	GLuint buffer;
	GLuint indexBuffer;
	int primitiveCount;
	float maxSize = 1;

	void load(std::string path)
	{
		objl::Loader model;
		model.LoadFile(path);

		std::vector<float> modelData;
		modelData.reserve(model.LoadedVertices.size() * 8);

		glm::vec3 minPos = glm::vec3(model.LoadedVertices[0].Position.X, model.LoadedVertices[0].Position.Y, model.LoadedVertices[0].Position.Z);
		glm::vec3 maxPos = glm::vec3(model.LoadedVertices[0].Position.X, model.LoadedVertices[0].Position.Y, model.LoadedVertices[0].Position.Z);
		for (auto &i : model.LoadedVertices)
		{
			float x = i.Position.X;
			float y = i.Position.Y;
			float z = i.Position.Z;

			if (x < minPos.x) { minPos.x = x; }
			if (y < minPos.y) { minPos.y = y; }
			if (z < minPos.z) { minPos.z = z; }

			if (x > maxPos.x) { maxPos.x = x; }
			if (y > maxPos.y) { maxPos.y = y; }
			if (z > maxPos.z) { maxPos.z = z; }
		}

		glm::vec3 modelSize = maxPos - minPos;
		glm::vec3 modelCentre = modelSize / 2.f;
		glm::vec3 modelDelta = modelCentre - maxPos;

		maxSize = std::max(modelSize.z, std::max(modelSize.x, modelSize.y));

		for (auto &i : model.LoadedVertices)
		{
			i.Position.X += modelDelta.x;
			i.Position.Y += modelDelta.y;
			i.Position.Z += modelDelta.z;
		}

		for (auto &i : model.LoadedVertices)
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

		for (auto &i : model.LoadedIndices)
		{
			modelIndices.push_back(i);
		}

		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, modelData.size() * sizeof(float), modelData.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelIndices.size() * sizeof(unsigned int), modelIndices.data(), GL_STATIC_DRAW);

		primitiveCount = modelIndices.size();
	}
};

Model models[LEVELS] = {};

void initLevelSelectorData()
{
	shader.fs = FragmentShader(RESOURCES_PATH "model.frag");
	shader.vs = VertexShader(RESOURCES_PATH "model.vert");
	shader.compileProgram();
	
	u_ModelViewProjection = getUniform(shader.id, "u_ModelViewProjection");
	u_modelToWorld = getUniform(shader.id, "u_modelToWorld");
	u_eyePosition = getUniform(shader.id, "u_eyePosition");
	u_lightPos = getUniform(shader.id, "u_lightPos");
	u_albedo = getUniform(shader.id, "u_albedo");
	u_color = getUniform(shader.id, "u_color");
	u_lightCount = getUniform(shader.id, "u_lightCount");
	
	colorTexture.loadFromFile(RESOURCES_PATH "colors.png");
	//colorTexture.loadFromFile(RESOURCES_PATH "ui/art.png");

	for(int i=0; i<LEVELS; i++)
	{
	
		models[i].load(RESOURCES_PATH "models/" + std::string(modelNames[i]));

	}


}

int currentLevelLooking = 0;
float rotation = 0;

float maxY = 2;
float minY = -0.5;

glm::vec3 eyePosition = glm::vec3{ 0,0.8,1.5 };
std::vector<glm::vec3> lightPositions = { glm::vec3(-0.5, 2.5, 1.2), glm::vec3(-0.2, 0.1, 4) };



void renderModel(float aspectRatio, int index, float scale, glm::vec3 pos, float color = 1)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	shader.bind();

	glBindBuffer(GL_ARRAY_BUFFER, models[index].buffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[index].indexBuffer);

	float size = (1.5 * scale) / models[index].maxSize;

	auto modelMatrix = getTransformMatrix(pos, {0, -rotation, 0},
		{ size, size, size });
	auto viewMatrix = glm::lookAt(eyePosition, {0, 0, 0}, { 0,1,0 });
	auto projectionMatrix = glm::perspective(glm::radians(90.f), aspectRatio, 0.01f, 100.f);

	glUniformMatrix4fv(u_ModelViewProjection, 1, GL_FALSE,
		&(projectionMatrix * viewMatrix * modelMatrix)[0][0]);

	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE,
		&(modelMatrix)[0][0]);

	glUniform3f(u_eyePosition, eyePosition.x, eyePosition.y, eyePosition.z);
	
	glUniform1i(u_albedo, 0);
	colorTexture.bind();

	glUniform1i(u_lightCount, lightPositions.size());
	glUniform1f(u_color, color);
	
	glUniform3fv(u_lightPos, lightPositions.size(), &lightPositions[0][0]);

	glDrawElements(GL_TRIANGLES, models[index].primitiveCount, GL_UNSIGNED_INT, 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

}

float ANIMATE_DURATION = 0.5;
float animateTime = 0;
int animateDirection = 0;

void enterMenu()
{

	animateTime = 0;
	animateDirection = 0;

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
	bool upHeld = input::isKeyHeld(input::Buttons::up);
	bool downHeld = input::isKeyHeld(input::Buttons::down);
	//bool escReleased = input::isKeyReleased(input::Buttons::esc);

	float fontSize = 1;
#pragma endregion

#pragma region handle input
	int changeDirection = 0;
	if (leftPressed && currentLevelLooking > 0)
	{
		changeDirection = -1;
	}else
	if (rightPressed && currentLevelLooking < LEVELS -1)
	{
		changeDirection = 1;
	}

	currentLevelLooking = glm::clamp(currentLevelLooking, 0, LEVELS - 1);


	if(acceptKeyReleased)
	{
		return currentLevelLooking;
	}

	float upDownSpeed = 1;
	if(upHeld)
	{
		eyePosition.y += deltaTime * upDownSpeed;
	}

	if (downHeld)
	{
		eyePosition.y -= deltaTime * upDownSpeed;
	}

	eyePosition.y = glm::clamp(eyePosition.y, minY, maxY);

#pragma endregion

#pragma region animate stuff

	rotation += deltaTime * glm::radians(35.f);

	if(rotation >= glm::radians(360.f))
	{
		rotation -= glm::radians(360.f);
	}


	if(changeDirection == 1)
	{
		animateTime = ANIMATE_DURATION;
		animateDirection = 1;
		currentLevelLooking += changeDirection;

	}else if (changeDirection == -1)
	{
		animateTime = ANIMATE_DURATION;
		animateDirection = -1;
		currentLevelLooking += changeDirection;
	}

	float scaleToDraw = 1;
	float animPos = 1;
	if(animateTime > 0)
	{
		scaleToDraw = animateTime/ANIMATE_DURATION;
		animPos = 1 - scaleToDraw;
		scaleToDraw = 1 - scaleToDraw;
		scaleToDraw /= 2.f;
		scaleToDraw += 0.5;


		animateTime -= deltaTime;
	
	}
	
	scaleToDraw = 1;
	animPos = glm::clamp(animPos, 0.f, 1.f);

	const glm::vec3 middleTarget = { 0,0,0 };
	const glm::vec3 leftTarget = { -2, 0, -2 };
	const glm::vec3 rightTarget = { 2, 0, -2 };

	glm::vec3 middlePos = middleTarget;
	glm::vec3 leftPos = leftTarget;
	glm::vec3 rightPos = rightTarget;

	float leftColor = 1 - animPos;
	float middleColor = animPos;
	float rightColor = 1 - animPos;

	if(animateDirection == 1)
	{
		leftPos = glm::mix(middleTarget, leftTarget, animPos);
		middlePos = glm::mix(rightTarget, middleTarget, animPos);
		rightPos = glm::mix({ rightTarget.x + 1, rightTarget.y, rightTarget.z }, rightTarget, animPos);
		rightColor = 0;
	}else if(animateDirection == -1)
	{
		leftPos = glm::mix({leftTarget.x-1,leftTarget.y, leftTarget.z}, leftTarget, animPos);
		middlePos = glm::mix(leftTarget, middleTarget, animPos);
		rightPos = glm::mix(middleTarget, rightTarget, animPos);
		leftColor = 0;
	}

	leftColor += 0.2; leftColor /= 1.2;
	middleColor += 0.2; middleColor /= 1.2;
	rightColor += 0.2; rightColor /= 1.2;

	if (currentLevelLooking > 0)
	{
		renderModel((float)renderer2d.windowW / renderer2d.windowH, currentLevelLooking-1, scaleToDraw, leftPos, leftColor);
	}

	if(currentLevelLooking < LEVELS-1)
	{
		renderModel((float)renderer2d.windowW / renderer2d.windowH, currentLevelLooking + 1, scaleToDraw, rightPos, rightColor);
	}

	renderModel((float)renderer2d.windowW / renderer2d.windowH, currentLevelLooking, scaleToDraw, middlePos, middleColor);

#pragma endregion


#pragma region UI

	{
		Ui::Frame fullScreenFrame({ 0,0, renderer2d.windowW, renderer2d.windowH });

		auto textSize = renderer2d.getTextSize(levelNames[currentLevelLooking], font, fontSize);

		auto textBox = Ui::Box().xCenter().yBottom(-textSize.y * 1.5)();

		renderer2d.renderText(glm::vec2(textBox), levelNames[currentLevelLooking], font, textColor, fontSize);

	}

#pragma endregion


	return -1;

}


