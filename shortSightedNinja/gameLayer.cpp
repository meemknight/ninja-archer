#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"
#include "mapData.h"
#include "math.h"
#include "Entity.h"
#include "input.h"
#include "Ui.h"

extern float gravitationalAcceleration;
extern float jumpSpeed;
extern float jumpFromWallSpeed;
extern float velocityClamp;
//extern float drag;
//extern float strafeSpeed = 10;
extern float runSpeed;
extern float airRunSpeed;
extern float grabMargin;
extern float notGrabTimeVal;
extern bool snapWallGrab;

extern gl2d::internal::ShaderProgram maskShader;
extern GLint maskSamplerUniform;

gl2d::Renderer2D renderer2d;
//gl2d::Renderer2D stencilRenderer2d;
gl2d::Renderer2D backgroundRenderer2d;
//sf::Music music;
MapRenderer mapRenderer;
MapData mapData;
Entity player;

#include "imgui.h"

gl2d::Texture sprites;
gl2d::Texture characterSprite;
gl2d::Texture targetSprite;
gl2d::Texture arrowSprite;
gl2d::Texture lightTexture;

//gl2d::FrameBuffer backGroundFBO;
gl2d::Texture backgroundTexture;

std::vector<Arrow> arrows;

std::vector<Pickup> pickups;

gl2d::Font font;

int currentArrow = Arrow::normalArrow;

const float arrowPickupCullDown = 5;

struct ArrowItem
{
	int type;
	int count;
	int maxCount;
};

//this vector should always have all arrows in order (and all of them there)
std::vector <ArrowItem> inventory;
std::vector <ArrowItem> actualInventorty;

struct LightSource
{
	static constexpr int animationStartTime = 2;
	glm::ivec2 pos;
	float animationDuration = animationStartTime;
};

std::vector <LightSource> wallLights;

void loadLevel()
{
	inventory.clear();
	//this vector should always have all arrows in order (and all of them there)
	inventory.push_back({ 0,3,5 });
	inventory.push_back({ 1,4,5 });
	inventory.push_back({ 2,5,5 });
	inventory.push_back({ 3,3,5 });

	renderer2d.currentCamera.zoom = 5.1;

	pickups.push_back({ 4, 4, 1 });

	mapData.create(40, 40,
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh<h"
		"hhhhhhhhhhhhhhhhhshhhhhshhhhhshhhhhshh<h"
		"hhhhhhhhhhhhhhhhhrhhhhhrhhhhhrhhhhhrhh<h"
		">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
		"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
	);

	mapData.get(1, 6).type = Block::waterFallBegin;
	mapData.get(1, 7).type = Block::waterFall;
	mapData.get(1, 8).type = Block::waterFallEnd;
	mapData.get(1, 9).type = Block::water4;
	mapData.get(1, 10).type = Block::water3;
	mapData.get(2, 9).type = Block::water1;
	mapData.get(2, 10).type = Block::water3;
	mapData.get(2, 10).type = '>';
	mapData.get(1, 12).type = '>';
	mapData.get(3, 8).type = Block::grassDecoration;
	mapData.get(5, 8).type = Block::leavesRight;
	mapData.get(7, 8).type = Block::leavesLeft;
	mapData.get(9, 8).type = Block::vines1;
	mapData.get(11, 8).type = Block::vines2;
	mapData.get(13, 8).type = Block::snowDecoration1;
	mapData.get(15, 8).type = Block::snowDecoration2;
	mapData.get(17, 8).type = Block::grassDecoration2;
	mapData.get(19, 8).type = Block::grassDecoration3;

	mapData.setNeighbors();

	player.pos = { BLOCK_SIZE * 4, BLOCK_SIZE * 4 };
	player.updateMove();
	player.dimensions = { 7, 7 };
	player.dying = 0;

	wallLights.clear();
	//setup light sources
	for (int y = 0; y < mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			if (isLitTorch(mapData.get(x, y).type))
			{
				wallLights.push_back({ {x,y}, 0 });
			}
		}
}


bool initGame()
{
	
	glClearColor(BACKGROUNDF_R, BACKGROUNDF_G, BACKGROUNDF_B ,1.f);


	renderer2d.create();
	//stencilRenderer2d.create();
	backgroundRenderer2d.create();
	backgroundRenderer2d.setShaderProgram(maskShader);
	//if (music.openFromFile("ding.flac"))
	//music.play();
	ShaderProgram sp{ "blocks.vert","blocks.frag" };

	mapRenderer.init(sp);
	sprites.loadFromFile("sprites2.png");

	mapRenderer.sprites = sprites;
	mapRenderer.upTexture.loadFromFile("top.png");
	mapRenderer.downTexture.loadFromFile("bottom.png");
	mapRenderer.leftTexture.loadFromFile("left.png");
	mapRenderer.rightTexture.loadFromFile("right.png");

	characterSprite.loadFromFile("character3.png");
	targetSprite.loadFromFile("target.png");
	arrowSprite.loadFromFile("arrow.png");
	lightTexture.loadFromFile("light.png");
	//backgroundTexture.loadFromFile("background.jpg");
	const char buff[] =
	{
		BACKGROUND_R,
		BACKGROUND_G,
		BACKGROUND_B,
		0xff
	};
	backgroundTexture.create1PxSquare(buff);

	//backGroundFBO.create(40 * BLOCK_SIZE, 40 * BLOCK_SIZE);

	font.createFromFile("font.ttf");
	
	arrows.reserve(10);

	loadLevel();

	return true;
}

bool gameLogic(float deltaTime)
{
	if (platform::isKeyPressedOn('T'))
	{
		loadLevel();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();

	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w, h);
	//stencilRenderer2d.updateWindowMetrics(backGroundFBO.texture.GetSize().x, 
	//	backGroundFBO.texture.GetSize().y);
	backgroundRenderer2d.updateWindowMetrics(w, h);

	//renderer2d.renderRectangle({ 100,100,100,100 }, Colors_Green);
	//renderer2d.flush();


	//renderer2d.currentCamera.position = { -500,-100 };

	if (player.wallGrab == 0)
	{
		if (player.grounded)
		{
			player.run(deltaTime * input::getMoveDir());
		}
		else
		{
			player.airRun(deltaTime * input::getMoveDir());
		}
	}

	if (platform::isKeyHeld('Z'))
	{
		renderer2d.currentCamera.zoom -= deltaTime;
	}
	if (platform::isKeyHeld('X'))
	{
		renderer2d.currentCamera.zoom += deltaTime;
	}


	if (input::isKeyPressedOn(input::Buttons::jump))
	{
		if (player.wallGrab == 0)
		{
			if (player.grounded)
			{
				player.jump();
			}
		}
		else if (player.wallGrab == 1)
		{
			player.strafe(-1);
			player.jumpFromWall();
			player.wallGrab = 0;
		}
		else if (player.wallGrab == -1)
		{
			player.strafe(1);
			player.jumpFromWall();
			player.wallGrab = 0;
		}
	}

	if (input::isKeyReleased(input::Buttons::jump))
	{
		if (player.velocity.y < 0)
		{
			player.velocity.y *= 0.4;
		}
	}

	actualInventorty.clear();

	for (auto i : inventory)
	{
		if (i.count)
		{
			actualInventorty.push_back(i);
		}
	}

	if (input::isKeyHeld(input::Buttons::down))
	{
		player.wallGrab = 0;
	}

	if(input::isKeyPressedOn(input::Buttons::shoot) && currentArrow > -1)
	{
		for(auto &i:inventory)
		{
			if(i.type == actualInventorty[currentArrow].type)
			{
				i.count--;
				break;
			}
		}

		Arrow a;
		a.type = (Arrow::ArrowTypes)actualInventorty[currentArrow].type;
		a.pos = player.pos + glm::vec2(player.dimensions.x / 2, player.dimensions.y / 2);
		a.shootDir = input::getShootDir({ w / 2,h / 2 });
		//a.pos.x += a.shootDir.x * BLOCK_SIZE * 0.9;
		//a.pos.y += a.shootDir.y * BLOCK_SIZE * 0.9;
		arrows.push_back(a);
	}

	//todo add player dimensions
	renderer2d.currentCamera.follow(player.pos + (player.dimensions / 2.f), deltaTime * 120, 30, renderer2d.windowW, renderer2d.windowH);
	//stencilRenderer2d.currentCamera = renderer2d.currentCamera;
	backgroundRenderer2d.currentCamera = renderer2d.currentCamera;

	player.applyGravity(deltaTime);
	player.applyVelocity(deltaTime);

	player.resolveConstrains(mapData);
	player.checkGrounded(mapData);
	player.checkWall(mapData, input::getMoveDir());
	player.updateMove();

	//mapRenderer.addBlock(renderer2d.toScreen({ 100,100,100,100 }), { 0,1,1,0 }, {1,1,1,1});
	//mapRenderer.render();

	mapData.clearColorData();

	simuleteLightSpot(player.pos + glm::vec2(player.dimensions.x/2, player.dimensions.y / 2),
		5, mapData, arrows, pickups, lightTexture, 0);

#pragma region inventory


	if (input::isKeyPressedOn(input::Buttons::swapLeft))
	{
		currentArrow--;
	}
	else if (input::isKeyPressedOn(input::Buttons::swapRight))
	{
		currentArrow++;
	}

	if (currentArrow < -1)
	{
		currentArrow = actualInventorty.size()-1;
	}

	if (currentArrow >= actualInventorty.size())
	{
		currentArrow = -1;
	}

	if(actualInventorty.size()==0)
	{
		currentArrow = -1;
	}

#pragma endregion

#pragma region lights

	{
		int minX = 0;
		int minY = 0;
		int maxX = mapData.w;
		int maxY = mapData.h;

		minX = (player.pos.x) / BLOCK_SIZE;
		maxX = (player.pos.x +player.dimensions.x) / BLOCK_SIZE;

		minY = (player.pos.y) / BLOCK_SIZE - 1;
		maxY = (player.pos.y + player.dimensions.y-1) / BLOCK_SIZE;

		minX = std::max(0, minX);
		minY = std::max(0, minY);
		maxX = std::min(mapData.w, maxX);
		maxY = std::min(mapData.h, maxY);

		for(int y=minY; y<=maxY;y++)
		{
			for (int x = minX; x <= maxX; x++)
			{
				if (unLitTorch(mapData.get(x, y).type))
				{
					mapData.get(x, y).type++;
					wallLights.push_back({{ x,y }});
				}

				mapData.get(x, y).playerEntered = 1;

				if(mapData.get(x,y).type == Block::water3)
				{
					player.dying = 1;
				}
			}
		}
	}

	for(auto &i: wallLights)
	{
		float r = 5;
		float intensity = 0.3;

		if(i.animationDuration <= 0)
		{
			r = 5;
			intensity = 0.3;
		}else
		{
			float perc;

			if(i.animationDuration > i.animationStartTime /2.f)
			{
				perc = i.animationDuration - (i.animationStartTime / 2.f);
				perc = perc/(i.animationStartTime / 2.f);
				perc = 1 - perc;
				r *= perc;
				intensity *= perc * 2;
				intensity = std::max(intensity, 0.3f);
			}else
			{
				perc = i.animationStartTime / 2.f - i.animationDuration;
				perc = perc / (i.animationStartTime / 2.f);
				perc = 1 - perc;
				intensity *=  (perc+1);
			}

			i.animationDuration -= deltaTime;
		}

		simuleteLightSpot({ i.pos.x*BLOCK_SIZE + BLOCK_SIZE/2,i.pos.y*BLOCK_SIZE + BLOCK_SIZE/2 },
			r, mapData, arrows, pickups, lightTexture, intensity);
	
	}

	for(auto &i: arrows)
	{
		if(i.type == Arrow::fireArrow)
		{
			float r = 5;

			if(i.liveTime < 5)
			{
				r *= (i.liveTime / 5.f );

				if(i.liveTime < 1)
				{
					r = 0;
				}
			}

			if(r > 0)
			{
				simuleteLightSpot({ i.pos },
					r, mapData, arrows, pickups, lightTexture, 0.1);
			}
			
		}
	
	}

#pragma endregion


#pragma region drawStencil

	//todo remove
	//stencilRenderer2d.clearDrawData();
	//stencilRenderer2d.flushFBO(backGroundFBO);
	//
	//backgroundRenderer2d.renderRectangle({ 0,0, mapData.w*BLOCK_SIZE, mapData.h*BLOCK_SIZE }, {}, 0, backgroundTexture);
	//glUseProgram(backgroundRenderer2d.currentShader.id);
	//glUniform1i(maskSamplerUniform, 1);
	//backGroundFBO.texture.bind(1);
	//backgroundRenderer2d.flush();
	//
	//backGroundFBO.clear();
#pragma endregion

#pragma region target
	{
		if(currentArrow!=-1)
		{
			glm::vec4 color = { 1,1,1,1 };

			switch (actualInventorty[currentArrow].type)
			{
			case Arrow::normalArrow :
				color = { 0.8,0.8,0.8,1 };
				break;
			case Arrow::fireArrow:
				color = { 0.9,0.0,0.0,1 };
				break;
			case Arrow::slimeArrow:
				color = { 0.0,0.9,0.0,1 };
				break;
			case Arrow::keyArrow:
				color = { 0.9,0.9,0.0,1 };
				break;
			default:
				break;
			}

			float fine = 0.9 * BLOCK_SIZE;
			glm::vec2 pos = player.pos + glm::vec2(player.dimensions.x / 2, player.dimensions.y / 2);
			glm::vec2 dir = input::getShootDir({ w / 2,h / 2 });
			float dist = BLOCK_SIZE * 10;
			for (int i = fine; i < BLOCK_SIZE * 10; i += fine)
			{
				pos += fine * dir;

				if (pos.x < 0
					|| pos.y < 0
					|| pos.x >(mapData.w)*BLOCK_SIZE
					|| pos.y >(mapData.h)*BLOCK_SIZE)
				{

				}
				else
				{
					if (isColidable(mapData.get(pos.x / BLOCK_SIZE, pos.y / BLOCK_SIZE).type))
					{
						dist = i;
						break;
					}
				}

				renderer2d.renderRectangle({ pos, 1,1 }, color);
			}
		}
		
	}
#pragma endregion

	{
		static int animPos;
		static float timePassed;
		timePassed += deltaTime;
		while (timePassed > 0.17)
		{
			timePassed -= 0.17;
			animPos++;
		}
		animPos %= 4;

		mapRenderer.drawFromMapData(renderer2d, mapData, deltaTime, animPos);
	}

#pragma region pickups
	for (auto &i : pickups)
	{
		i.draw(renderer2d, arrowSprite, deltaTime);
		i.light = 0;

		if(i.colidePlayer(player))
		{
			i.cullDown = arrowPickupCullDown;
			inventory[i.type].count = inventory[i.type].maxCount;
		}
	}
	
	actualInventorty.clear();

	for (auto i : inventory)
	{
		if (i.count)
		{
			actualInventorty.push_back(i);
		}
	}

#pragma endregion


	player.draw(renderer2d, deltaTime, characterSprite);

#pragma region arrows
	for(auto i=0; i<arrows.size(); i++)
	{
		auto &a = arrows[i];
		if(a.leftMap(mapData.w, mapData.h) || a.timeOut(deltaTime))
		{
			arrows.erase(arrows.begin() + i);
			i--;
			continue;
		}

		a.move(deltaTime * BLOCK_SIZE);
		a.draw(renderer2d, arrowSprite);
		a.checkCollision(mapData);
		a.light = 0;
	}
	
#pragma endregion

	{
		auto c = renderer2d.currentCamera;
		renderer2d.currentCamera.setDefault();

		Ui::Frame f({ 0,0, w,h });


		{
			Ui::Frame cornerLeft(Ui::Box().xLeft(20).yBottom(-20).xDimensionPercentage(0.1f).yAspectRatio(0.5f)());

			int centerCount = 1;
			int leftCount = 1;
			int rightCount = 1;

			if (actualInventorty.size() != 0)
			{

				int left = currentArrow - 1;
				if(left==-1)
				{
				}else
				{
					if (left < -1)
					{
						left = actualInventorty.size() - 1;
					}

					leftCount = actualInventorty[left].count;
					left = actualInventorty[left].type;
				}

				if(actualInventorty.size()==1)
				{
					left = -1;
				}
				
				int center = currentArrow;
				if (center <= -1)
				{
					
				}
				else
				{
					centerCount = actualInventorty[center].count;
					center = actualInventorty[center].type;
				}


				int right = currentArrow + 1;;
				if(right >= actualInventorty.size())
				{
					right = -1;
				}else
				{
					rightCount = actualInventorty[right].count;
					right = actualInventorty[right].type;
				}

				//todo change current arrow
				if(left!=-1)
				{
					for (int i = -1; i < leftCount - 1; i++)
					{
						renderer2d.renderRectangle(
							Ui::Box().xLeft(i*8).yCenter().yDimensionPercentage(0.7f).xAspectRatio(1)
							, { 0.4,0.4,0.4,1 }
						, {}, 45, arrowSprite, gl2d::computeTextureAtlas(4, 1, left, 0));
					}
				}
			

				if(right!=-1)
				{
					for (int i = -1; i < rightCount - 1; i++)
					{
						renderer2d.renderRectangle(
							Ui::Box().xRight(i*8).yCenter().yDimensionPercentage(0.7f).xAspectRatio(1)
							, { 0.4,0.4,0.4,1 }
						, {}, 45, arrowSprite, gl2d::computeTextureAtlas(4, 1, right, 0));
					}
				}
		

				if(center!=-1)
				{
				
					for(int i=-1; i<centerCount-1;i++)
					{
						renderer2d.renderRectangle(
							Ui::Box().xCenter(i*10).yCenter().yDimensionPercentage(0.9f).xAspectRatio(1)
							, {}, 45, arrowSprite, gl2d::computeTextureAtlas(4, 1, center, 0));
					}


				//renderer2d.renderText(Ui::Box().xCenter().yBottom()(),
				//	"1/2", font, { 1,1,1,1 }, 0.5);

				}

			}

		}
		

		
		renderer2d.currentCamera = c;
	}

	renderer2d.flush();

	return true;

}

void closeGame()
{
	//music.stop();
}

void imguiFunc(float deltaTime)
{

	static bool active = 0;
	static glm::vec4 color;

	//todo delta time
	/*
	extern float gravitationalAcceleration;
extern float jumpSpeed;
extern float jumpFromWallSpeed;
extern float velocityClamp;
//extern float drag;
//extern float strafeSpeed = 10;
extern float runSpeed;
extern float airRunSpeed;
extern float grabMargin;
extern float notGrabTimeVal;
extern bool snapWallGrab;
	*/

	//ImGui::Begin("delta");
	//ImGui::Text(std::to_string(1.f/(deltaTime/1000.f)).c_str());
	//ImGui::End();

	//ImGui::Begin("Move settings");
	//ImGui::SliderFloat("gravitationalAcceleration", &gravitationalAcceleration, 30, 100);
	//ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 1, 50);
	//ImGui::SliderFloat("jumpFromWallSpeed", &jumpFromWallSpeed, 1, 50);
	//ImGui::SliderFloat("velocityClamp", &velocityClamp, 10, 70);
	//ImGui::SliderFloat("runSpeed", &runSpeed, 1, 40);
	//ImGui::SliderFloat("airRunSpeed", &airRunSpeed, 1, 40);
	//ImGui::SliderFloat("grabMargin", &grabMargin, 0, 1);
	//ImGui::Checkbox("snapWallGrab", &snapWallGrab);
	//
	//gl2d::TextureAtlas spriteAtlas(BLOCK_COUNT, 4);
	//
	//int mCount = 0;
	//while(mCount < (int)Block::lastBlock)
	//{
	//	
	//		ImGui::Image((void*)(intptr_t)sprites.id,
	//			{ 120,120 }, { spriteAtlas.get(mCount, 0).x, spriteAtlas.get(mCount,0).y }, { spriteAtlas.get(mCount, 0).z, spriteAtlas.get(mCount,0).w }, { 1,1,1,1 }, { 1,0,0,1 });
	//		
	//	if(mCount %5 != 0)
	//	ImGui::SameLine();
	//	
	//	mCount++;
	//}
	//	
	//ImGui::End();
	//
	//ImGui::Begin("My First Tool", &active, ImGuiWindowFlags_MenuBar);
	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Close", "Ctrl+W")) { active = false; }
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMenuBar();
	//}
	//
	//// Edit a color (stored as ~4 floats)
	//ImGui::ColorEdit4("Color", &color[0]);
	//
	//// Plot some values
	//const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
	//ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));
	//
	//// Display contents in a scrolling region
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	//ImGui::BeginChild("Scrolling");
	//for (int n = 0; n < 50; n++)
	//	ImGui::Text("%04d: Some text", n);
	//ImGui::EndChild();
	//ImGui::End();

}
