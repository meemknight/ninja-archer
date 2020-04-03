#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"
#include "mapData.h"
#include "math.h"
#include "Entity.h"
#include "input.h"

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

gl2d::Renderer2D renderer2d;
//sf::Music music;
MapRenderer mapRenderer;
MapData mapData;
Entity player;

#include "imgui.h"

gl2d::Texture sprites;
gl2d::Texture characterSprite;

bool initGame()
{
	renderer2d.create();
	//if (music.openFromFile("ding.flac"))
	//music.play();
	ShaderProgram sp{ "blocks.vert","blocks.frag" };
	sprites.loadFromFile("sprites.png");
	characterSprite.loadFromFile("character.png");

	mapRenderer.init(sp);
	mapRenderer.sprites = sprites;
	mapRenderer.upTexture.loadFromFile("top.png");
	mapRenderer.downTexture.loadFromFile("bottom.png");
	mapRenderer.leftTexture.loadFromFile("left.png");
	mapRenderer.rightTexture.loadFromFile("right.png");

	mapData.create(40, 40, 
		"!!!!!!!!!                             !!"
		"!!!!!!!!!!!!!!!!                      !!"
		"!!!!!!!!!!!!!              !          !!"
		"!!!                        !          !!"
		"!!!                        !          !!"
		"!!!!!!!!!!!!!!!!           !!!!!!!!!!!!!"
		"!!!!!!!!               !      !       !!"
		"!!!!!!                 !     !!       !!"
		"!!            !!!!!!!!!!!!!!!!!       !!"
		"!!          !!!                       !!"
		"!!                                  !!!!"
		"!!!!!!!!!!              !           !!!!"
		"!!                      !           !!!!"
		"!!            !!!!!!!!!!!!!!!!!!!!!!!!!!"
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
	player.dimensions = { 24, 30 };

	mapData.ConvertTileMapToPolyMap();

	return true;
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();

	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w, h);

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

	if (platform::isKeyHeld('Q'))
	{
		renderer2d.currentCamera.zoom -= deltaTime;
	}
	if (platform::isKeyHeld('E'))
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


	//todo add player dimensions
	renderer2d.currentCamera.follow(player.pos + (player.dimensions / 2.f), deltaTime * 120, 30, renderer2d.windowW, renderer2d.windowH);

	player.applyGravity(deltaTime);
	player.applyVelocity(deltaTime);

	player.resolveConstrains(mapData);
	player.checkGrounded(mapData);
	player.checkWall(mapData, input::getMoveDir());
	player.updateMove();

	//mapRenderer.addBlock(renderer2d.toScreen({ 100,100,100,100 }), { 0,1,1,0 }, {1,1,1,1});
	//mapRenderer.render();

	mapData.clearColorData();

	std::vector<glm::vec2> triangles;

	//simuleteLightTrace({ player.pos.x + player.dimensions.x / 2, player.pos.y+8 },
	//	100, mapData, triangles);

	//for (int x = 0; x < 40; x++)
	//{
	//	for (int y = 0; y < 40; y++)
	//	{
	//		mapData.get(x, y).mainColor = { 1,1,1,1 };
	//		mapData.get(x, y).sideColors = { 1,1,1,1 };
	//	}
	//}

	simuleteLightSpot(player.pos, 44, mapData, triangles);

	mapRenderer.drawFromMapData(renderer2d, mapData);

	gl2d::TextureAtlas playerAtlas(1, 1);

	renderer2d.renderRectangle({ player.pos, player.dimensions }, {}, 0, characterSprite,
		playerAtlas.get(0, 0, !player.movingRight));

	glm::vec2 cursorPos = input::getShootDir({ w / 2,h / 2 });
	cursorPos *= BLOCK_SIZE * 2;
	cursorPos += player.pos;
	cursorPos += glm::vec2{player.dimensions.x / 2, player.dimensions.y /2};

	renderer2d.renderRectangle({ cursorPos, 4, 4 }, Colors_Green);

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
