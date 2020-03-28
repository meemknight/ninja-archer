#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"

gl2d::Renderer2D renderer2d;
//sf::Music music;
MapRenderer map;

bool initGame()
{
	renderer2d.create();
	//if (music.openFromFile("ding.flac"))
	//	music.play();
	
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

	map.addBlock({ -0.5,0.5,0.5, - 0.5 });
	map.render();

	return true;
}

void closeGame()
{
	//music.stop();
}
