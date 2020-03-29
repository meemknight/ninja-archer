#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"

gl2d::Renderer2D renderer2d;
//sf::Music music;
MapRenderer map;

#include "imgui.h"

bool initGame()
{
	renderer2d.create();
	//if (music.openFromFile("ding.flac"))
	//music.play();
	
	map.init({ "blocks.vert","blocks.frag" });

	return true;
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();

	renderer2d.updateWindowMetrics(w,h);

	//renderer2d.renderRectangle({ 100,100,100,100 }, Colors_Green);
	//renderer2d.flush();

	float speed = 500;
	
	if (input::isKeyHeld('W'))
	{
		renderer2d.currentCamera.position.y -= deltaTime * speed;
	}
	if (input::isKeyHeld('S'))
	{
		renderer2d.currentCamera.position.y += deltaTime * speed;
	}
	if (input::isKeyHeld('A'))
	{
		renderer2d.currentCamera.position.x -= deltaTime * speed;
	}
	if (input::isKeyHeld('D'))
	{
		renderer2d.currentCamera.position.x += deltaTime * speed;
	}
	if (input::isKeyHeld('Q'))
	{
		renderer2d.currentCamera.zoom-= deltaTime;
	}
	if (input::isKeyHeld('E'))
	{
		renderer2d.currentCamera.zoom += deltaTime;
	}

	map.addBlock(renderer2d.toScreen({100,100,100,200}));
	map.render();

	return true;

}

void closeGame()
{
	//music.stop();
}


void imguiFunc()
{

	static bool active = 0;

	ImGui::Begin("My First Tool", &active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { active = false; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	static glm::vec4 color;
	// Edit a color (stored as ~4 floats)
	ImGui::ColorEdit4("Color", &color[0]);

	// Plot some values
	const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
	ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

	// Display contents in a scrolling region
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	ImGui::BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		ImGui::Text("%04d: Some text", n);
	ImGui::EndChild();
	ImGui::End();

}
