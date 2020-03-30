#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"
#include "mapData.h"
#include "math.h"
#include "Entity.h"

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

	mapData.create(40, 40, 
		"                                        "
		"                                        "
		"                                        "
		"                                        "
		"                                        "
		"                                 !!     "
		"                                 !!     "
		"                                        "
		"                                        "
		"                  !              !!!!!!!"
		"                  !              !!!!!!!"
		" !!!!!!!!!!!!!!!!!!!!!!!!!!  !!!!!!!!!!!"
		"                        !    !          "
		"                        !    !          "
		"                        !    !        !!"
		"                        !    !          "
		"                        !    !!!        "
		"                   !    !    !          "
		"                   !         !       !!!"
		"                   !         !          "
		"                   !         !!!!       "
		" !!!!!  !!!!!!!!!!!!!!!!!!!!!!          "
		"                                      !!"
		"                                        "
		"                                        "
		"                                        "
		"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!      !"
		"!                         !            !"
		"!                         !          !!!"
		"!!!!            !         !   !!!       "
		"!            !!!!!!!      !             "
		"!               !       !!!           !!"
		"!!!!!!!                   !    !!!!     "
		"                                        "
		"          !!                            "
		"              !!                        "
		"!!!!        !!!!!!!!       !!!!!!!!!!!!!"
		"                                        "
		"                       !!               "
		" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	);

	player.pos = { 200, 200 };
	player.dimensions = { 24, 30 };

	return true;
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();

	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w,h);

	//renderer2d.renderRectangle({ 100,100,100,100 }, Colors_Green);
	//renderer2d.flush();

	float speed = 200;

	//renderer2d.currentCamera.position = { -500,-100 };

	if (input::isKeyHeld('W'))
	{
		player.pos.y -= deltaTime * speed;
	}
	if (input::isKeyHeld('S'))
	{
		player.pos.y += deltaTime * speed;
	}
	if (input::isKeyHeld('A'))
	{
		player.pos.x -= deltaTime * speed;
	}
	if (input::isKeyHeld('D'))
	{
		player.pos.x += deltaTime * speed;
	}
	if (input::isKeyHeld('Q'))
	{
		renderer2d.currentCamera.zoom-= deltaTime;
	}
	if (input::isKeyHeld('E'))
	{
		renderer2d.currentCamera.zoom += deltaTime;
	}

	if(input::isKeyPressedOn(VK_SPACE))
	{
		player.jump();
	}

	//todo add player dimensions
	renderer2d.currentCamera.follow(player.pos + (player.dimensions/2.f), deltaTime * 120, 30, renderer2d.windowW, renderer2d.windowH );

	player.applyGravity(deltaTime);
	player.applyVelocity(deltaTime);

	player.checkCollision(mapData);
	player.updateMove();

	//mapRenderer.addBlock(renderer2d.toScreen({ 100,100,100,100 }), { 0,1,1,0 }, {1,1,1,1});
	//mapRenderer.render();

	mapData.clearColorData();

	std::vector<glm::vec2> triangles;

	simulateLight({ 80,80 }, mapData, triangles);

	mapRenderer.drawFromMapData(renderer2d ,mapData);

	gl2d::TextureAtlas playerAtlas(1, 1);

	renderer2d.renderRectangle({ player.pos, player.dimensions }, {}, 0, characterSprite,
		playerAtlas.get(0,0, !player.movingRight));
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
	//todo xinput and input

	//ImGui::Begin("delta");
	//ImGui::Text(std::to_string(1.f/(deltaTime/1000.f)).c_str());
	//
	//ImGui::End();
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
