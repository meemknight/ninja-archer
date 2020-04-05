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
gl2d::Renderer2D stencilRenderer2d;
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

gl2d::FrameBuffer backGroundFBO;
gl2d::Texture backgroundTexture;

std::vector<Arrow> arrows;

int currentArrow = Arrow::normalArrow;

struct Item
{
	int type;
	int count;
	int maxCount;
};

std::vector <Item> inventory;

struct LightSource
{
	static constexpr int animationStartTime = 2;
	glm::ivec2 pos;
	float animationDuration = animationStartTime;
};

std::vector <LightSource> wallLights;

bool initGame()
{
	inventory.push_back({ 0,1,1 });
	inventory.push_back({ 1,1,1 });
	inventory.push_back({ 2,1,1 });

	renderer2d.create();
	stencilRenderer2d.create();
	backgroundRenderer2d.create();
	backgroundRenderer2d.setShaderProgram(maskShader);
	//if (music.openFromFile("ding.flac"))
	//music.play();
	ShaderProgram sp{ "blocks.vert","blocks.frag" };
	sprites.loadFromFile("sprites.png");
	characterSprite.loadFromFile("character.png");
	targetSprite.loadFromFile("target.png");
	arrowSprite.loadFromFile("arrow.png");
	lightTexture.loadFromFile("light.png");
	backgroundTexture.loadFromFile("background.jpg");
	backGroundFBO.create(40 * BLOCK_SIZE, 40 * BLOCK_SIZE);

	mapRenderer.init(sp);
	mapRenderer.sprites = sprites;
	mapRenderer.upTexture.loadFromFile("top.png");
	mapRenderer.downTexture.loadFromFile("bottom.png");
	mapRenderer.leftTexture.loadFromFile("left.png");
	mapRenderer.rightTexture.loadFromFile("right.png");

	mapData.create(40, 40, 
		"!!!!!!!!!                             !!"
		"!!!!!!!!!!!!!!!2                      !!"
		"!!!!!!!!!!!!!              '          !!"
		"!!!                 +      *        (!!!"
		"!!!                 +      *        !!!!"
		"!!!!!!!!#######$***********!!!!,,,,,!!!!"
		"!!!!!!!!33333333       +      -3333333!!"
		"!!!!!!3333333333       +     --3333333!!"
		"!!333333333333!!!!!!!!!!!!!!!!!3333333!!"
		"!!3353336333!!!33333333333333333333333!!"
		"!!3333333333333333333333333333331111!!!!"
		"!!!!!!!!!!33333333333333233333331333!!!!"
		"!!3333333333333333333333!33333331333!!!!"
		"!!3353335333533353333333!!!!!!!!!!!!!!!!"
		"!!333333333333333333333333333333333333!!"
		"!!!!!!!!!!!!!!!!!!!!!!!!!             !!"
		"!!          !         !        !      !!"
		"!!    !!!        !!!       !!!      !!!!"
		"!!      !          !         !        !!"
		"!!!!        !!!       !!!      !!!    !!"
		"!!   !  !       !  !      !  !     !  !!"
		"!!!!!!  !!  !!!!!  !! !!!!!  !!!!!!!  !!"
		"!!                                    !!"
		"!!                                    !!"
		"!!  !!  ! !!!  !!  ! !!  !!  ! !  !!  !!"
		"!!                                    !!"
		"!!          !         !        !      !!"
		"!!    !!!        !!!       !!!      !!!!"
		"!!      !          !         !        !!"
		"!!!!        !!!       !!!      !!!    !!"
		"!!   !  !       !  !      !  !     !  !!"
		"!!!!!!  !!  !!!!!  !! !!!!!  !!!!!!!  !!"
		"!!                                    !!"
		"!!                                    !!"
		"!!  !!  ! !!!  !!  ! !!  !!  ! !  !!  !!"
		"!!                                    !!"
		"!!          !         !        !      !!"
		"!!    !!!        !!!       !!!      !!!!"
		"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	);

	player.pos = { BLOCK_SIZE*4, BLOCK_SIZE*4 };
	player.updateMove();
	player.dimensions = { 20, 30 };

	//todo remove
	mapData.ConvertTileMapToPolyMap();

	arrows.reserve(10);

	//setup light sources
	for(int y=0; y<mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			if(isLitTorch(mapData.get(x,y).type))
			{
				wallLights.push_back({ {x,y}, 0});
			}
		}

	return true;
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();

	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w, h);
	stencilRenderer2d.updateWindowMetrics(backGroundFBO.texture.GetSize().x, 
		backGroundFBO.texture.GetSize().y);
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
			//player.strafe(-1);
			player.jumpFromWall();
			player.wallGrab = 0;
		}
		else if (player.wallGrab == -1)
		{
			//player.strafe(1);
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

	if (input::isKeyHeld(input::Buttons::down))
	{
		player.wallGrab = 0;
	}

	if(input::isKeyPressedOn(input::Buttons::shoot))
	{
		Arrow a;
		a.type = (Arrow::ArrowTypes)currentArrow;
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
		12, mapData, arrows, stencilRenderer2d, lightTexture, 0);

	if (input::isKeyPressedOn(input::Buttons::swapLeft))
	{
		currentArrow--;
		if (currentArrow < 0)
		{
			currentArrow = Arrow::lastArror - 1;
		}

	}
	else if (input::isKeyPressedOn(input::Buttons::swapRight))
	{
		currentArrow++;
		if (currentArrow >= Arrow::lastArror)
		{
			currentArrow = 0;
		}
	}


#pragma region lights

	{
		int minX = 0;
		int minY = 0;
		int maxX = mapData.w;
		int maxY = mapData.h;

		minX = (player.pos.x) / BLOCK_SIZE;
		maxX = (player.pos.x +player.dimensions.x) / BLOCK_SIZE;

		minY = (player.pos.y) / BLOCK_SIZE;
		maxY = (player.pos.y + player.dimensions.y) / BLOCK_SIZE;

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
				ilog(perc);
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
			r, mapData, arrows, stencilRenderer2d, lightTexture, intensity);
	
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
					r, mapData, arrows, stencilRenderer2d, lightTexture, 0.1);
			}
			
		}
	
	}

#pragma endregion


#pragma region drawStencil

	stencilRenderer2d.flushFBO(backGroundFBO);
	
	backgroundRenderer2d.renderRectangle({ 0,0, mapData.w*BLOCK_SIZE, mapData.h*BLOCK_SIZE }, {}, 0, backgroundTexture);
	glUseProgram(backgroundRenderer2d.currentShader.id);
	glUniform1i(maskSamplerUniform, 1);
	backGroundFBO.texture.bind(1);
	backgroundRenderer2d.flush();
	
	backGroundFBO.clear();
#pragma endregion

#pragma region target
	{
		float fine = 1 * BLOCK_SIZE;
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
				
			}else
			{
				if (isColidable(mapData.get(pos.x / BLOCK_SIZE, pos.y / BLOCK_SIZE).type))
				{
					dist = i;
					break;
				}
			}
			
			renderer2d.renderRectangle({ pos, 2,2 }, { 1,1,1,0.8 });
		}

		
	}
#pragma endregion

	mapRenderer.drawFromMapData(renderer2d, mapData);

	gl2d::TextureAtlas playerAtlas(1, 1);

	renderer2d.renderRectangle({ player.pos - glm::vec2(2,0),  24, 30  }, {}, 0, characterSprite,
		playerAtlas.get(0, 0, !player.movingRight));

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

		renderer2d.renderRectangle(
			Ui::Box().xLeft(20).yBottom(-20).yDimensionPercentage(0.1f).xAspectRatio(1)
			, {}, 45, arrowSprite, gl2d::computeTextureAtlas(4,1, currentArrow,0));
		
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

	ImGui::Begin("Move settings");
	ImGui::SliderFloat("gravitationalAcceleration", &gravitationalAcceleration, 30, 100);
	ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 1, 50);
	ImGui::SliderFloat("jumpFromWallSpeed", &jumpFromWallSpeed, 1, 50);
	ImGui::SliderFloat("velocityClamp", &velocityClamp, 10, 70);
	ImGui::SliderFloat("runSpeed", &runSpeed, 1, 40);
	ImGui::SliderFloat("airRunSpeed", &airRunSpeed, 1, 40);
	ImGui::SliderFloat("grabMargin", &grabMargin, 0, 1);
	ImGui::Checkbox("snapWallGrab", &snapWallGrab);

	ImGui::End();

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
