#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"
#include "mapData.h"
#include "math.h"
#include "Entity.h"
#include "input.h"
#include <fstream>
#include "imgui.h"
#pragma region Macros

#define BACKGROUND_R 33
#define BACKGROUND_G 38
#define BACKGROUND_B 63

#define BACKGROUNDF_R ((float)33 / (float)0xff)
#define BACKGROUNDF_G ((float)38 / (float)0xff)
#define BACKGROUNDF_B ((float)63 / (float)0xff)

#pragma endregion

#pragma region Variables
extern gl2d::internal::ShaderProgram maskShader;
extern GLint maskSamplerUniform;

gl2d::Renderer2D renderer2d;
//sf::Music music;
MapRenderer mapRenderer;
MapData mapData;

gl2d::Texture sprites;
gl2d::Texture arrowSprite;

gl2d::FrameBuffer backGroundFBO;
gl2d::Texture backgroundTexture;

std::vector<Arrow> arrows;
std::vector<signData> signs;
std::vector<doorData> doors;
std::vector<torchData> torches;

unsigned short currentBlock = Block::blueNoSolid1;

int mapWidth, mapHeight;
char mapName[256] = {};
unsigned short* map = nullptr;

bool collidable = true;
bool nonCollidable = true;
bool showBoxes = false;
bool showDangers = false;
bool simulateLights = false;
bool simulateUnlitLights = false;
bool highlightCheckPoints = false;
bool editItems = false;
char signStr[255] = {};
glm::ivec2 itemPos;
int levelId = -2;
float torchLight = 5;
#pragma endregion

float getTorchLight(int x, int y)
{
	auto iter = std::find_if(torches.begin(), torches.end(),
		[x, y](torchData &d)->bool {return (d.pos.x == x && d.pos.y == y); });

	if (iter != torches.end())
	{
		return iter->light;
	}
	else
	{
		return 5;
	}
}

bool initGame()
{
	renderer2d.create();
	ShaderProgram sp{ "resources//blocks.vert","resources//blocks.frag" };
	sprites.loadFromFile("resources//sprites.png");
	backGroundFBO.create(40 * BLOCK_SIZE, 40 * BLOCK_SIZE);

	mapRenderer.init(sp);
	mapRenderer.sprites = sprites;
	mapRenderer.upTexture.loadFromFile("resources//top.png");
	mapRenderer.downTexture.loadFromFile("resources//bottom.png");
	mapRenderer.leftTexture.loadFromFile("resources//left.png");
	mapRenderer.rightTexture.loadFromFile("resources//right.png");

	mapData.create(40, 40, map);

	mapWidth = mapData.w;
	mapHeight = mapData.h;

	glClearColor(BACKGROUNDF_R, BACKGROUNDF_G, BACKGROUNDF_B, 1.f);

	return true;
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int w = getWindowSizeX();
	int h = getWindowSizeY();

	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w, h);


#pragma region Camera Movement
	if (platform::isKeyHeld('Q'))
	{
		renderer2d.currentCamera.zoom -= deltaTime * 3;
	}
	if (platform::isKeyHeld('E'))
	{
		renderer2d.currentCamera.zoom += deltaTime * 3;
	}
	if (platform::isKeyHeld('D'))
	{
		renderer2d.currentCamera.position.x += deltaTime * 220;
	}
	if (platform::isKeyHeld('A'))
	{
		renderer2d.currentCamera.position.x -= deltaTime * 220;
	}
	if (platform::isKeyHeld('W'))
	{
		renderer2d.currentCamera.position.y -= deltaTime * 220;
	}
	if (platform::isKeyHeld('S'))
	{
		renderer2d.currentCamera.position.y += deltaTime * 220;
	}
#pragma endregion

	mapData.clearColorData();

#pragma region Adding Blocks Into the World
	if (editItems)
	{
		if (platform::isLMouseButtonPressed())
		{
			glm::vec2 mousePos;
			mousePos.x = (platform::getRelMousePosition().x + renderer2d.currentCamera.position.x);
			mousePos.y = (platform::getRelMousePosition().y + renderer2d.currentCamera.position.y);

			mousePos = gl2d::scaleAroundPoint(mousePos, renderer2d.currentCamera.position +
				glm::vec2{ renderer2d.windowW / 2, renderer2d.windowH / 2 }, 1.f / renderer2d.currentCamera.zoom);
			if (mousePos.x / BLOCK_SIZE < 0 || (mousePos.x) / BLOCK_SIZE >= mapData.w
				|| mousePos.y / BLOCK_SIZE < 0 || (mousePos.y) / BLOCK_SIZE >= mapData.h)
			{

			}
			else
			{
				auto block = mapData.get(mousePos.x / BLOCK_SIZE, mousePos.y / BLOCK_SIZE).type;

#pragma region Edit Signs
				if (isSign(block))
				{
					auto it = std::find_if(signs.begin(), signs.end(),
						[x = (int)mousePos.x / BLOCK_SIZE, y = (int)mousePos.y / BLOCK_SIZE](signData& d)
					{ return d.pos.x == x && d.pos.y == y; });
					if (it != signs.end())
					{
						int j = 0;
						for (auto i : it->text)
						{
							signStr[j++] = i;
						}
						signStr[j] = NULL;

						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
					else
					{
						signStr[0] = 0;
						std::string s = std::string(signStr);
						glm::ivec2 pos = { mousePos.x / BLOCK_SIZE, mousePos.y / BLOCK_SIZE };
						signData d(pos, s);
						signs.emplace_back(d);

						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
				}
#pragma endregion

#pragma region Edit Door
				if (isDoor(block))
				{
					auto it = std::find_if(doors.begin(), doors.end(),
						[x = (int)mousePos.x / BLOCK_SIZE, y = (int)mousePos.y / BLOCK_SIZE](doorData& d)
					{ return d.pos.x == x && d.pos.y == y; });

					if (it != doors.end())
					{
						levelId = it->levelId;
						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
					else
					{
						glm::ivec2 pos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
						doorData d(pos, -2);
						doors.emplace_back(d);

						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
				}
#pragma endregion

#pragma region Edit Torch
				if (isLitTorch(block) ||
					unLitTorch(block))
				{
					auto it = std::find_if(torches.begin(), torches.end(),
						[x = (int)mousePos.x / BLOCK_SIZE, y = (int)mousePos.y / BLOCK_SIZE](torchData& d)
					{ return d.pos.x == x && d.pos.y == y; });

					if (it != torches.end())
					{
						torchLight = it->light;
						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
					else
					{
						glm::ivec2 pos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
						torchData d(pos, 5);
						torches.emplace_back(d);

						itemPos = { (int)mousePos.x / BLOCK_SIZE, (int)mousePos.y / BLOCK_SIZE };
					}
				}
#pragma endregion
			}
		}
	}
#pragma region Eye Dropper Tool
	else if (platform::isKeyHeld(VK_CONTROL))
	{
		if (platform::isLMouseButtonPressed() || platform::isRMouseButtonPressed())
		{
			glm::vec2 mousePos;
			mousePos.x = platform::getRelMousePosition().x + renderer2d.currentCamera.position.x;
			mousePos.y = platform::getRelMousePosition().y + renderer2d.currentCamera.position.y;

			mousePos = gl2d::scaleAroundPoint(mousePos, renderer2d.currentCamera.position +
				glm::vec2{ renderer2d.windowW / 2, renderer2d.windowH / 2 }, 1.f / renderer2d.currentCamera.zoom);
			if (mousePos.x / BLOCK_SIZE < 0 || (mousePos.x) / BLOCK_SIZE >= mapData.w
				|| mousePos.y / BLOCK_SIZE < 0 || (mousePos.y) / BLOCK_SIZE >= mapData.h)
			{

			}
			else
			{
				if (mapData.get(mousePos.x / BLOCK_SIZE, (mousePos.y) / BLOCK_SIZE).type != Block::none)
					currentBlock = mapData.get(mousePos.x / BLOCK_SIZE, (mousePos.y) / BLOCK_SIZE).type;
			}
		}
	}
#pragma endregion

	else
	{
#pragma region Place Blocks
		if (platform::isLMouseHeld())
		{
			glm::vec2 mousePos;
			mousePos.x = platform::getRelMousePosition().x + renderer2d.currentCamera.position.x;
			mousePos.y = platform::getRelMousePosition().y + renderer2d.currentCamera.position.y;

			mousePos = gl2d::scaleAroundPoint(mousePos, renderer2d.currentCamera.position +
				glm::vec2{ renderer2d.windowW / 2, renderer2d.windowH / 2 }, 1.f / renderer2d.currentCamera.zoom);

			if ((mousePos.x) / BLOCK_SIZE < 0 || (mousePos.x) / BLOCK_SIZE >= mapData.w
				|| (mousePos.y) / BLOCK_SIZE < 0 || (mousePos.y) / BLOCK_SIZE >= mapData.h)
			{

			}
			else
			{
				if (currentBlock == Block::flagUp)
				{
					for (int x = 0; x < mapData.w; x++)
						for (int y = 0; y < mapData.h; y++)
						{
							if (mapData.get(x, y).type == Block::flagUp)
							{
								mapData.get(x, y).type = Block::flagDown;
							}
						}
				}

				mapData.get(mousePos.x / BLOCK_SIZE, mousePos.y / BLOCK_SIZE).type = currentBlock;

			}

		}
#pragma endregion

#pragma region Eraser
		else if (platform::isRMouseHeld())
		{
			glm::vec2 mousePos;
			mousePos.x = platform::getRelMousePosition().x + renderer2d.currentCamera.position.x;
			mousePos.y = platform::getRelMousePosition().y + renderer2d.currentCamera.position.y;

			mousePos = gl2d::scaleAroundPoint(mousePos, renderer2d.currentCamera.position +
				glm::vec2{ renderer2d.windowW / 2, renderer2d.windowH / 2 }, 1.f / renderer2d.currentCamera.zoom);
			if (mousePos.x / BLOCK_SIZE < 0 || (mousePos.x) / BLOCK_SIZE >= mapData.w
				|| mousePos.y / BLOCK_SIZE < 0 || (mousePos.y) / BLOCK_SIZE >= mapData.h)
			{

			}
			else
			{
				mapData.get((mousePos.x) / BLOCK_SIZE, (mousePos.y) / BLOCK_SIZE).type = Block::none;
			}

		}
#pragma endregion

#pragma region Render the current block
		else
		{
			glm::vec2 mousePos;
			mousePos.x = platform::getRelMousePosition().x + renderer2d.currentCamera.position.x;
			mousePos.y = platform::getRelMousePosition().y + renderer2d.currentCamera.position.y;

			mousePos = gl2d::scaleAroundPoint(mousePos, renderer2d.currentCamera.position +
				glm::vec2{ renderer2d.windowW / 2, renderer2d.windowH / 2 }, 1.f / renderer2d.currentCamera.zoom);
			if (mousePos.x / BLOCK_SIZE < 0 || (mousePos.x) / BLOCK_SIZE >= mapData.w
				|| mousePos.y / BLOCK_SIZE < 0 || (mousePos.y) / BLOCK_SIZE >= mapData.h)
			{

			}
			else
			{
				gl2d::TextureAtlas spriteAtlas(BLOCK_COUNT, 4);

				renderer2d.renderRectangle({ (int)(mousePos.x / BLOCK_SIZE) * BLOCK_SIZE , (int)(mousePos.y / BLOCK_SIZE) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE },
					{}, 0, sprites, { spriteAtlas.get(currentBlock - 1, 0).x, spriteAtlas.get(currentBlock - 1,0).y,
					 spriteAtlas.get(currentBlock - 1, 0).z, spriteAtlas.get(currentBlock - 1,0).w });
			}
		}
#pragma endregion

	}

#pragma endregion

#pragma region Render the blocks

	if (simulateLights || simulateUnlitLights)
	{
		for (int x = 0; x < mapData.w; x++)
			for (int y = 0; y < mapData.h; y++)
			{
				if (isLitTorch(mapData.get(x, y).type) && simulateLights)
				{
					float light = getTorchLight(x, y);
					simuleteLightSpot({ x * BLOCK_SIZE + BLOCK_SIZE / 2,y * BLOCK_SIZE + BLOCK_SIZE / 2 },
						light, mapData);
				}

				if (unLitTorch(mapData.get(x, y).type) && simulateUnlitLights)
				{
					float light = getTorchLight(x, y);
					simuleteLightSpot({ x * BLOCK_SIZE + BLOCK_SIZE / 2,y * BLOCK_SIZE + BLOCK_SIZE / 2 },
						light, mapData);
				}

			}
	}
	else
	{
		for (int x = 0; x < mapData.w; x++)
			for (int y = 0; y < mapData.h; y++)
			{
				mapData.get(x, y).mainColor = { 1,1,1,1 };
			}
	}

	for (int x = 0; x < mapData.w; x++)
	{
		for (int y = 0; y < mapData.h; y++)
		{
			auto light = mapData.get(x, y).mainColor.w;
			mapData.get(x, y).mainColor.w = 0;
			if (collidable) {
				if (isColidable(mapData.get(x, y).type))
				{
					mapData.get(x, y).mainColor.w = 1;
				}
			}
			else
			{
				if (isColidable(mapData.get(x, y).type))
				{
					mapData.get(x, y).mainColor.w = 0.2;
				}

			}

			if (nonCollidable)
			{
				if (!isColidable(mapData.get(x, y).type))
				{
					mapData.get(x, y).mainColor.w = 1;
				}
			}
			else
			{
				if (!isColidable(mapData.get(x, y).type))
				{
					mapData.get(x, y).mainColor.w = 0.2;
				}
			}

			//mapData.get(x, y).mainColor.w *= light;
		}
	}

	mapRenderer.drawFromMapData(renderer2d, mapData);
#pragma endregion

#pragma region Render Map Margins
	renderer2d.renderRectangle({ 0,0, mapData.w * BLOCK_SIZE, -10 }, Colors_Turqoise);
	renderer2d.renderRectangle({ 0,0,-10, mapData.h * BLOCK_SIZE }, Colors_Turqoise);
	renderer2d.renderRectangle({ 0,mapData.h * BLOCK_SIZE, mapData.w * BLOCK_SIZE, 10 }, Colors_Turqoise);
	renderer2d.renderRectangle({ mapData.w * BLOCK_SIZE,0,10,mapData.h * BLOCK_SIZE }, Colors_Turqoise);
#pragma endregion

#pragma region ImGui Check Boxes
	for (int y = 0; y < mapData.h; y++)
	{
		for (int x = 0; x < mapData.w; x++)
		{
			if (isColidable(mapData.get(x, y).type) && showBoxes)
			{
				renderer2d.renderRectangle({ x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 0.5,0.1,0.5,0.2 });
			}
			else if (mapData.get(x, y).type == Block::water3 && showDangers)
			{
				renderer2d.renderRectangle({ x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 1,0.2,0.2,0.9 });
			}

			if (mapData.get(x, y).type == Block::flagDown && highlightCheckPoints)
			{
				renderer2d.renderRectangle({ x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 0,1,0.6,0.7 });
			}

			if (mapData.get(x, y).type == Block::flagUp && highlightCheckPoints)
			{
				renderer2d.renderRectangle({ x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 0,1,0.2,0.9 });
			}

			if (isSign(mapData.get(x, y).type && editItems))
			{
				// show signs
			}

			
		}

	}

	if (editItems)
	{
		renderer2d.renderRectangle({ itemPos.x * BLOCK_SIZE, itemPos.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE }, { 1,1,0.2,0.4 });
	}
#pragma endregion

	renderer2d.flush();

	return true;
}

void closeGame()
{
	//music.stop();
}


void imguiFunc(float deltaTime)
{
	ImGui::Begin("Map settings");

#pragma region Open Map
	static char name[256] = {};
	ImGui::InputText("OutputFile Name", name, sizeof(name));
	if (ImGui::Button("Open Map"))
	{
		char aux[256];
		strcpy(aux, "resources//");
		strcat(aux, name);
		strcat(aux, ".level");
		std::ifstream inputFile(aux);

		inputFile >> mapWidth >> mapHeight;

		unsigned short blocks[500 * 500];
		int it = 0;
		std::string current;

		for (int i = 0; i < mapHeight; i++)
		{
			std::getline(inputFile, current);
			for (auto i = 0; i < current.length(); i++)
			{
				std::string number;
				while (current[i] != ',')
				{
					number += current[i];
					i++;
				}
				blocks[it++] = static_cast<unsigned short>(std::stoi(number));
			}
		}
		blocks[it] = NULL;

		doors.clear();
		signs.clear();

		while (std::getline(inputFile, current))
		{
			if (current[0] == 0)
			{
				continue;
			}

			if (current[3] == 's' && current[4] == 'i')
			{
				int x, y;
				std::string str;
				sscanf(current.c_str(), "md.signDataVector.emplace_back( glm::ivec2{%d, %d}", &x, &y);

				int i;
				for (i = 0; i < current.length(); i++)
				{
					if (current[i] == '"')
					{
						i++;
						break;
					}
				}
				while (current[i] != '"')
				{
					str += current[i++];
				}

				signData d({ x, y }, str);
				signs.emplace_back(d);
			}
			else if (current[3] == 'e')
			{
				int x, y, id;
				sscanf(current.c_str(), "md.exitDataVector.emplace_back(glm::ivec2{%d, %d}, %d);", &x, &y, &id);
				doorData d({ x, y }, id);
				doors.emplace_back(d);
			}
			else if (current[3] == 't')
			{
				int x, y;
				float light;
				sscanf(current.c_str(), "md.torchDataVector.emplace_back(glm::ivec2{%d, %d}, %f);", &x, &y, &light);
				torchData d({ x, y }, light);
				torches.emplace_back(d);
			}
		}


		inputFile.close();

		mapData.cleanup();
		mapData.create(mapWidth, mapHeight, blocks);
	}
	ImGui::NewLine();
#pragma endregion

#pragma region Empty Map
	ImGui::InputInt("New Map Width", &mapWidth);
	ImGui::InputInt("New Map Height", &mapHeight);

	if (ImGui::Button("New Map"))
	{
		MapData temp;
		temp.create(mapWidth, mapHeight, nullptr);

		for (int x = 0; x < mapWidth && x < mapData.w; x++)
			for (int y = 0; y < mapHeight && y < mapData.h; y++)
			{
				temp.get(x, y) = mapData.get(x, y);
			}

		mapData.cleanup();
		mapData = temp;
	}
	ImGui::NewLine();
#pragma endregion

#pragma region Save Map

	if (ImGui::Button("Save Map"))
	{
		char aux[256] = {};
		strcpy(aux, "resources//");
		strcat(aux, name);
		strcat(aux, ".level");

		std::ofstream outputFile(aux);
		outputFile << mapWidth << std::endl << mapHeight << std::endl;
		for (int y = 0; y < mapHeight; y++)
		{
			for (int x = 0; x < mapWidth; x++)
			{
				outputFile << static_cast<int>(mapData.get(x, y).type) << ",";
			}
			outputFile << "\n";
		}

		outputFile << "\n\n";



		for (auto& i : signs)
		{
			if (isSign(mapData.get(i.pos.x, i.pos.y).type))
			{
				outputFile << "md.signDataVector.emplace_back( glm::ivec2{" << i.pos.x << ", " << i.pos.y << "}, \"" << i.text << "\");\n";
			}
		}

		outputFile << "\n\n";

		for (auto& i : doors)
		{
			if (isDoor(mapData.get(i.pos.x, i.pos.y).type))
			{
				outputFile << "md.exitDataVector.emplace_back(glm::ivec2{" << i.pos.x << ", " << i.pos.y << "}, " << i.levelId << ");\n";
			}
		}

		outputFile << "\n\n";

		for (auto& i : torches)
		{
			if (isLitTorch(mapData.get(i.pos.x, i.pos.y).type)||
				unLitTorch(mapData.get(i.pos.x, i.pos.y).type))
			{
				outputFile << "md.torchDataVector.emplace_back(glm::ivec2{ " << i.pos.x << ", " << i.pos.y << "}, " << i.light << ");\n";
			}
		}

		outputFile.close();
	}

	ImGui::NewLine();
#pragma endregion

#pragma region Block Selector
	ImGui::Checkbox("Edit items", &editItems);

	if (editItems)
	{
		ImGui::InputText("Sign Text", signStr, sizeof(signStr));
#pragma region Edit Sign


		if (ImGui::Button("Save Sign"))
		{
			auto it = std::find_if(signs.begin(), signs.end(),
				[x = itemPos.x, y = itemPos.y](signData& d)
			{ return d.pos.x == x && d.pos.y == y; });
			it->text = std::string(signStr);
		}
#pragma endregion
		ImGui::NewLine();
#pragma region Edit Door
		ImGui::InputInt("Next Level Id", &levelId);

		if (ImGui::Button("Save Door"))
		{
			auto it = std::find_if(doors.begin(), doors.end(),
				[x = itemPos.x, y = itemPos.y](doorData& d)
			{ return d.pos.x == x && d.pos.y == y; });
			it->levelId = levelId;
		}
#pragma endregion
		ImGui::NewLine();
#pragma region Edit Torch
		ImGui::InputFloat("Torch", &torchLight);

		if (ImGui::Button("Save Torch"))
		{
			auto it = std::find_if(torches.begin(), torches.end(),
				[x = itemPos.x, y = itemPos.y](torchData& d)
			{ return d.pos.x == x && d.pos.y == y; });

			it->light = torchLight;
		}
#pragma endregion

	}
	ImGui::NewLine();

	ImGui::Checkbox("Show Collidable Blocks", &collidable);
	ImGui::Checkbox("Show Non-Collidable Blocks", &nonCollidable);
	ImGui::Checkbox("Highlight Boxes", &showBoxes);

	ImGui::Checkbox("Highlight Dangers", &showDangers); ImGui::SameLine();
	ImGui::Checkbox("Simulate Lights", &simulateLights); ImGui::SameLine();
	ImGui::Checkbox("Simulate Unlit Lights", &simulateUnlitLights); ImGui::SameLine();
	ImGui::Checkbox("Highlight Check points", &highlightCheckPoints);

	gl2d::TextureAtlas spriteAtlas(BLOCK_COUNT, 4);
	unsigned short mCount = 1;
	ImGui::BeginChild("Block Selector");

	if (collidable && nonCollidable)
	{
		unsigned short localCount = 0;
		while (mCount < Block::lastBlock)
		{
			if (!isUnfinished(mCount))
			{
				ImGui::PushID(mCount);
				if (ImGui::ImageButton((void*)(intptr_t)sprites.id,
					{ 35,35 }, { spriteAtlas.get(mCount - 1, 0).x, spriteAtlas.get(mCount - 1,0).y },
					{ spriteAtlas.get(mCount - 1, 0).z, spriteAtlas.get(mCount - 1,0).w }))
				{
					currentBlock = mCount;
				}
				ImGui::PopID();

				if (localCount % 10 != 0)
				{
					ImGui::SameLine();
				}
				localCount++;
			}

			mCount++;
		}
	}
	else
	{
		if (collidable && !nonCollidable)
		{
			unsigned short localCount = 0;
			while (mCount < Block::lastBlock)
			{
				if (isColidable(mCount) && !isUnfinished(mCount))
				{
					ImGui::PushID(mCount);
					if (ImGui::ImageButton((void*)(intptr_t)sprites.id,
						{ 35,35 }, { spriteAtlas.get(mCount - 1, 0).x, spriteAtlas.get(mCount - 1,0).y },
						{ spriteAtlas.get(mCount - 1, 0).z, spriteAtlas.get(mCount - 1,0).w }))
					{
						currentBlock = mCount;
					}
					ImGui::PopID();

					if (localCount % 10 != 0)
						ImGui::SameLine();
					localCount++;
				}
				mCount++;
			}
		}
		else if (!collidable && nonCollidable)
		{
			unsigned short localCount = 0;
			while (mCount < Block::lastBlock)
			{
				if (!isColidable(mCount) && !isUnfinished(mCount))
				{
					ImGui::PushID(mCount);
					if (ImGui::ImageButton((void*)(intptr_t)sprites.id,
						{ 35,35 }, { spriteAtlas.get(mCount - 1, 0).x, spriteAtlas.get(mCount - 1,0).y },
						{ spriteAtlas.get(mCount - 1, 0).z, spriteAtlas.get(mCount - 1,0).w }))
					{
						currentBlock = mCount;
						//llog((int)currentBlock);
					}
					ImGui::PopID();

					if (localCount % 10 != 0)
						ImGui::SameLine();
					localCount++;
				}
				mCount++;
			}
		}
	}
	ImGui::EndChild();

#pragma endregion

	ImGui::End();
}
