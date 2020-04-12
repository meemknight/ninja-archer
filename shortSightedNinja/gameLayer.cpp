#include "gameLayer.h"
#include "opengl2Dlib.h"
#include <SFML/Audio.hpp>
#include "mapRenderer.h"
#include "mapData.h"
#include "math.h"
#include "Entity.h"
#include "input.h"
#include "Ui.h"
#include "Particle.h"
#include <algorithm>
#include <string>
#include <sstream>

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

//music
sf::SoundBuffer pickupSoundbuffer;
sf::SoundBuffer leavesSoundbuffer;
sf::Sound soundPlayer;
sf::Music waterPlayer;
sf::Music tikiPlayer;
sf::Music greenPlayer;
sf::Music redPlayer;
sf::Music grayPlayer;

enum Sounds
{
	none
	//todo
};

MapRenderer mapRenderer;
MapData mapData;
Entity player;

#include "imgui.h"

gl2d::Texture sprites;
gl2d::Texture characterSprite;
gl2d::Texture arrowSprite;
gl2d::Texture particlesSprite;
gl2d::Texture crackTexture;

gl2d::Texture uiFrame;
gl2d::Texture uiForest;
gl2d::Texture uiTiki;
gl2d::Texture uiCastle;
gl2d::Texture uiCave;
gl2d::Texture uiMountain;
gl2d::Texture uiSnowMountain;
gl2d::Texture uiArrows;
gl2d::Texture uiButton;
gl2d::Texture uiItch;
gl2d::Texture uiMusic;
gl2d::Texture uiArt;

std::vector<Arrow> arrows;

std::vector<Pickup> pickups;

gl2d::Font font;

int currentArrow = Arrow::normalArrow;

const float arrowPickupCullDown = 5;

glm::ivec2 playerSpawnPos = { 0,0 };

Particle jumpParticle;
std::vector<Particle>crackParticles;

// -2 if is main menu
int currentLevel=-2;

float jumpDelayTime = 0;

struct ArrowItem
{
	int type;
	int count;
	int maxCount;
};

//this vector should always have all arrows in order (and all of them there)
std::vector <ArrowItem> inventory;
std::vector <ArrowItem> actualInventorty;

std::vector <LightSource> wallLights;

float playerLight = 6;

void respawn();

void loadLevel()
{
	inventory.clear();
	//this vector should always have all arrows in order (and all of them there)
	inventory.push_back({ 0,0,3 });
	inventory.push_back({ 1,0,3 });
	inventory.push_back({ 2,0,3 });
	inventory.push_back({ 3,0,3 });
	inventory.push_back({ 4,0,3 });

	wallLights.clear();

	renderer2d.currentCamera.zoom = 5.1;

	pickups.clear();
	arrows.clear();

	//pickups.push_back({ 4, 4, 1 });

	if(currentLevel==-2)
	{
		mapData.cleanup();
		return;
	}

	setupMap(mapData, currentLevel);
	
	for (int y = 0; y < mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			if (mapData.get(x, y).type == Block::flagUp)
			{
				playerSpawnPos = { x,y };
			}

			if (mapData.get(x, y).type == Block::woddenArrow)
			{
				pickups.push_back({ x, y, 0 });
				mapData.get(x, y).type = Block::none;
			}
			if (mapData.get(x, y).type == Block::fireArrow)
			{
				pickups.push_back({ x, y, 1 });
				mapData.get(x, y).type = Block::none;
			}
			if (mapData.get(x, y).type == Block::slimeArrow)
			{
				pickups.push_back({ x, y, 2 });
				mapData.get(x, y).type = Block::none;
			}
			if (mapData.get(x, y).type == Block::keyArrow)
			{
				pickups.push_back({ x, y, 3 });
				mapData.get(x, y).type = Block::none;
			}if (mapData.get(x, y).type == Block::bombArrow)
			{
				pickups.push_back({ x, y, 4 });
				mapData.get(x, y).type = Block::none;
			}
		}

	player.pos = { BLOCK_SIZE * playerSpawnPos.x, BLOCK_SIZE * playerSpawnPos.y };
	//player.updateMove();
	player.lastPos = player.pos;

	player.dimensions = { 7, 7 };
	player.dying = 0;
	player.lockMovementDie = 0;
	playerLight = 5;
	player.velocity = {};
	player.isExitingLevel = -1;
	player.wallGrab = 0;

	wallLights.clear();
	//setup light sources
	for (int y = 0; y < mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			if (isLitTorch(mapData.get(x, y).type))
			{
				wallLights.push_back({ {x,y}, 0, mapData.getTorchLight(x,y) });
			}
		}

	if(mapData.waterPos.size() > 0)
	{
		waterPlayer.play();
		waterPlayer.setVolume(0);
	}

	if (mapData.tikiSoundPos.size() > 0)
	{
		tikiPlayer.play();
		tikiPlayer.setVolume(0);
	}

	if (mapData.greenSoundPos.size() > 0)
	{
		greenPlayer.play();
		greenPlayer.setVolume(0);
	}

	if (mapData.redSoundPos.size() > 0)
	{
		redPlayer.play();
		redPlayer.setVolume(0);
	}

	if (mapData.caveSoundPos.size() > 0)
	{
		grayPlayer.play();
		grayPlayer.setVolume(0);
	}

	saveState(playerSpawnPos, currentLevel);
	respawn();
}

void respawn()
{
	inventory.clear();
	//this vector should always have all arrows in order (and all of them there)
	inventory.push_back({ 0,0,3 });
	inventory.push_back({ 1,0,3 });
	inventory.push_back({ 2,0,3 });
	inventory.push_back({ 3,0,3 });

	arrows.clear();

	player.pos = { BLOCK_SIZE * playerSpawnPos.x, BLOCK_SIZE * playerSpawnPos.y };
	player.updateMove();
	player.dimensions = { 7, 7 };
	player.dying = 0;
	player.lockMovementDie = 0;
	playerLight = 5;
	player.velocity = {};

}

bool initGame()
{
	srand(time(0));

	font.createFromFile("resources//font.ttf");

	glClearColor(BACKGROUNDF_R, BACKGROUNDF_G, BACKGROUNDF_B, 1.f);

	renderer2d.create();
	//stencilRenderer2d.create();
	//if (music.openFromFile("ding.flac"))
	//music.play();
	ShaderProgram sp{ "resources//blocks.vert","resources//blocks.frag" };

	mapRenderer.init(sp);
	sprites.loadFromFileWithPixelPadding("resources//sprites.png", BLOCK_SIZE);

	mapRenderer.sprites = sprites;
	mapRenderer.upTexture.loadFromFile("resources//top.png");
	mapRenderer.downTexture.loadFromFile("resources//bottom.png");
	mapRenderer.leftTexture.loadFromFile("resources//left.png");
	mapRenderer.rightTexture.loadFromFile("resources//right.png");

	characterSprite.loadFromFileWithPixelPadding("resources//character.png", 8);
	//todo replace with padding
	arrowSprite.loadFromFile("resources//arrow.png");
	particlesSprite.loadFromFileWithPixelPadding("resources//particles.png", 8);
	crackTexture.loadFromFileWithPixelPadding("resources//crackAnim.png", 8);

	uiFrame.loadFromFile("resources//ui//uiFrame.png");
	uiForest.loadFromFile("resources//ui//forest.png");
	uiTiki.loadFromFile("resources//ui//tikiForest.png");
	uiCastle.loadFromFile("resources//ui//castle.png");
	uiCave.loadFromFile("resources//ui//cave.png");
	uiMountain.loadFromFile("resources//ui//mountain.png");
	uiSnowMountain.loadFromFile("resources//ui//snowMountain.png");
	uiArrows.loadFromFile("resources//ui//arrow.png");
	uiButton.loadFromFile("resources//ui//button.png");
	uiItch.loadFromFile("resources//ui//itch.png");
	uiMusic.loadFromFile("resources//ui//music.jpg");
	uiArt.loadFromFile("resources//ui//art.png");

	const char buff[] =
	{
		BACKGROUND_R,
		BACKGROUND_G,
		BACKGROUND_B,
		0xff
	};

	jumpParticle.animCount = 3;

	arrows.reserve(10);


	{//music
		pickupSoundbuffer.loadFromFile("resources//pick_up.wav");
		leavesSoundbuffer.loadFromFile("resources//leaves.wav");
	
		waterPlayer.openFromFile("resources//water.wav");
		waterPlayer.setLoop(1);
		
		tikiPlayer.openFromFile("resources//tikiForest.wav");
		tikiPlayer.setLoop(1);

		greenPlayer.openFromFile("resources//rainForest.wav");
		greenPlayer.setLoop(1);

		redPlayer.openFromFile("resources//jungle.wav");
		redPlayer.setLoop(1);

		grayPlayer.openFromFile("resources//cave.wav");
		grayPlayer.setLoop(1);

		soundPlayer.setVolume(2);
	}

	//if (loadLevelFromLastState(currentLevel, playerSpawnPos))
	//{
	//	glm::ivec2 i = playerSpawnPos;
	//	loadLevel();
	//	playerSpawnPos = i;
	//	respawn();
	//}else
	{
		currentLevel = -2;
		//loadLevel();
	}

	return true;
}

enum MenuState :int
{
	mainMenu = 1,
	levelSelector,
	creditsAres,

}; int menuState = MenuState::mainMenu;

bool isInButton(const glm::vec2 &p, const glm::vec4 &box)
{
	return(p.x >= box.x && p.x <= box.x + box.z
		&&
		p.y >= box.y && p.y <= box.y + box.w
		);
}

bool isButtonReleased(const glm::vec2 &p, const glm::vec4 &box)
{
	return(p.x >= box.x && p.x <= box.x + box.z
		&&
		p.y >= box.y && p.y <= box.y + box.w
		) && platform::isLMouseButtonReleased();
}

bool gameLogic(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int w, h;
	w = getWindowSizeX();
	h = getWindowSizeY();
	glViewport(0, 0, w, h);
	renderer2d.updateWindowMetrics(w, h);

#pragma region MyRegion

	static int selectedLevel;

	if(input::isKeyReleased(input::Buttons::esc))
	{
		menuState = MenuState::mainMenu;
	}

	//main menu
	if (currentLevel == -2)
	{

		waterPlayer.stop();
		greenPlayer.stop();
		tikiPlayer.stop();
		redPlayer.stop();
		grayPlayer.stop();
		soundPlayer.stop();
		waterPlayer.setVolume(0);
		greenPlayer.setVolume(0);
		redPlayer.setVolume(0);
		grayPlayer.setVolume(0);
		tikiPlayer.setVolume(0);

		renderer2d.currentCamera = gl2d::cameraCreateDefault();

		if(menuState == MenuState::mainMenu)
		{
			Ui::Frame f({ 0, 0, w, h });
			
			auto button1 = Ui::Box().xCenter().yCenter( - 200).xDimensionPercentage(0.5).yDimensionPixels(150)();
			auto button2 = Ui::Box().xCenter().yCenter().xDimensionPercentage(0.5).yDimensionPixels(150)();
			auto button3 = Ui::Box().xCenter().yCenter(+200).xDimensionPercentage(0.5).yDimensionPixels(150)();

			auto p = platform::getRelMousePosition();
			
			bool playButton = 0;
			bool levelSelectButton = 0;
			bool creditsSelectButton = 0;

			//todo refactor
			if (isInButton(p, button1))
			{
				if (platform::isLMouseButtonReleased())
				{
					playButton = 1;
				}
			}

			if (isInButton(p, button2))
			{
				if (platform::isLMouseButtonReleased())
				{
					levelSelectButton = 1;
				}
			}

			if (isInButton(p, button3))
			{
				if (platform::isLMouseButtonReleased())
				{
					creditsSelectButton = 1;
				}
			}

			renderer2d.render9Patch2( button1,
				8, { 1,1,1,1 }, {}, 0, uiButton, {0,1,1,0}, { 0,0.8,0.8,0 });

			renderer2d.render9Patch2(button2,
				8, { 1,1,1,1 }, {}, 0, uiButton, { 0,1,1,0 }, { 0,0.8,0.8,0 });

			renderer2d.render9Patch2(button3,
				8, { 1,1,1,1 }, {}, 0, uiButton, { 0,1,1,0 }, { 0,0.8,0.8,0 });


			renderer2d.renderText({button1.x + button1.z/2,button1.y + button1.w/2}, 
				"Continue jurney", font, {1,1,1,1}, 0.7);


			renderer2d.renderText({ button2.x + button2.z / 2,button2.y + button2.w / 2 },
				"Select zone", font, { 1,1,1,1 }, 0.7);

			renderer2d.renderText({ button3.x + button3.z / 2,button3.y + button3.w / 2 },
				"Credits", font, { 1,1,1,1 }, 0.7);

			if (playButton)
			{
				
				if(loadLevelFromLastState(currentLevel, playerSpawnPos))
				{
					glm::ivec2 i = playerSpawnPos;
					loadLevel();
					playerSpawnPos = i;
					saveState(i, currentLevel);
					respawn();
				}else
				{
					currentLevel = 0;
					loadLevel();
					respawn();
				}

			}

			if(levelSelectButton)
			{
				menuState = MenuState::levelSelector;
				selectedLevel = 0;
			}

			if(creditsSelectButton)
			{
				menuState = MenuState::creditsAres;
			}

		}else if (menuState == MenuState::levelSelector)
		{

			Ui::Frame f({ 0, 0, w, h });

			glm::vec4 frame2 = Ui::Box().xCenter().yTop(50).yDimensionPercentage(0.6).xAspectRatio(1.f);

			renderer2d.renderRectangle(
				frame2,
				{}, 0, uiFrame);

			{
				Ui::Frame f(frame2);

				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiForest);
				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiCastle);
				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiCave);
				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiMountain);
				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiSnowMountain);
				renderer2d.renderRectangle(
					Ui::Box().xCenter().yCenter().yDimensionPercentage(0.6).xAspectRatio(1.f),
					{}, 0, uiTiki);


				uiArrows;
			}

			glm::vec4 playBox= Ui::Box().xCenter().yBottom(-20).yDimensionPixels(100).xDimensionPercentage(0.8);
			
			std::string temp(std::string("Play ") + std::to_string(selectedLevel + 1));

			renderer2d.render9Patch2(playBox,
				8, { 1,1,1,1 }, {}, 0, uiButton, { 0,1,1,0 }, { 0,0.8,0.8,0 });
			renderer2d.renderText({ playBox.x + playBox.z /2 ,playBox.y + playBox.w / 2 },
				temp.c_str(), font, { 1,1,1,1 }, 0.7);

			gl2d::TextureAtlas arrowsAtlas(1, 2);
			int leftPressed = 0;
			int rightPressed = 0;

			int lReleased = 0;
			int rReleased = 0;

			auto leftBox = Ui::Box().xCenter(-frame2.z / 1.5).yCenter().yDimensionPercentage(0.2).xAspectRatio(1.f)();
			auto rightBox = Ui::Box().xCenter(frame2.z / 1.5).yCenter().yDimensionPercentage(0.2).xAspectRatio(1.f)();

			auto p = platform::getRelMousePosition();

#pragma region buttons

			if (platform::isLMouseHeld())
			{
				if (p.x >= leftBox.x && p.x <= leftBox.x + leftBox.z
					&&
					p.y >= leftBox.y && p.y <= leftBox.y + leftBox.w
					)
				{
					
					leftPressed = 1;
				}

				if (p.x >= rightBox.x && p.x <= rightBox.x + rightBox.z
					&&
					p.y >= rightBox.y && p.y <= rightBox.y + rightBox.w
					)
				{
					
					rightPressed = 1;
				}
			}
			
			{
				if (p.x >= leftBox.x && p.x <= leftBox.x + leftBox.z
					&&
					p.y >= leftBox.y && p.y <= leftBox.y + leftBox.w
					)
				{
					if (platform::isLMouseButtonReleased())
					{
						lReleased = 1;
					}
				}

				if (p.x >= rightBox.x && p.x <= rightBox.x + rightBox.z
					&&
					p.y >= rightBox.y && p.y <= rightBox.y + rightBox.w
					)
				{
					if (platform::isLMouseButtonReleased())
					{
						rReleased = 1;
					}
				}
			}

#pragma endregion 

			if(selectedLevel >0)
			renderer2d.renderRectangle(leftBox,
				{}, 0, uiArrows, arrowsAtlas.get(0, leftPressed, 1));

			if (selectedLevel < LEVELS-1)
			renderer2d.renderRectangle(rightBox,
				{}, 0, uiArrows, arrowsAtlas.get(0, rightPressed));

			if (lReleased)
			{

				selectedLevel--;
				if(selectedLevel <0)
				{
					selectedLevel = 0;
				}
			
				//currentLevel = 2;
				//loadLevel();
			}
			if (rReleased)
			{
				selectedLevel++;
				if (selectedLevel >= LEVELS)
				{
					selectedLevel = LEVELS-1;
				}
				//currentLevel = 2;
				//loadLevel();
			}

			if(isButtonReleased(p, playBox))
			{
				currentLevel = selectedLevel;
				loadLevel();
			}

		}else if (menuState == MenuState::creditsAres)
		{
			auto p = platform::getRelMousePosition();

			Ui::Frame f({ 0, 0, w, h });

			auto button1 = Ui::Box().xLeft(100).yCenter(-200).yDimensionPixels(150).xAspectRatio(1)();
			auto button2 = Ui::Box().xLeft(100).yCenter().yDimensionPixels(150).xAspectRatio(1)();
			auto button3 = Ui::Box().xLeft(100).yCenter(+200).yDimensionPixels(150).xAspectRatio(1)();
			
			renderer2d.render9Patch2(button1,
				8, { 1,1,1,1 }, {}, 0, uiItch, { 0,1,1,0 }, { 0,0.8,0.8,0 });

			renderer2d.render9Patch2(button2,
				8, { 1,1,1,1 }, {}, 0, uiMusic, { 0,1,1,0 }, { 0,0.8,0.8,0 });

			renderer2d.render9Patch2(button3,
				8, { 1,1,1,1 }, {}, 0, uiArt, { 0,1,1,0 }, { 0,0.8,0.8,0 });


			renderer2d.renderText({ button1.x + button1.z + 200 ,button1.y + button1.w / 2 },
				"Our page", font, { 1,1,1,1 }, 0.7);


			renderer2d.renderText({ button2.x + button2.z + 200 ,button2.y + button2.w / 2 },
				"Music", font, { 1,1,1,1 }, 0.7);

			renderer2d.renderText({ button3.x + button3.z + 200 ,button3.y + button3.w / 2 },
				"Art", font, { 1,1,1,1 }, 0.7);
		

		https://wuxia.itch.io/
			if (isButtonReleased(p, button1)) {system("start https://wuxia.itch.io/"); };
			if (isButtonReleased(p, button2)) {system("start https://www.youtube.com/channel/UCEXX5i6961zc4-L8thTctBg");};
			if (isButtonReleased(p, button3)) {system("start https://itch.io/profile/adamatomic"); };

		}


		renderer2d.flush();
		return 1;
	}  ////////////////////////////////////////// end main menu

#pragma endregion

	//if (platform::isKeyPressedOn('T'))
	//{
	//	loadLevel();
	//}

	if (player.dying || player.isExitingLevel != -1)
	{
		playerLight -= deltaTime * 2;
		if (playerLight < 1)
		{
			if(player.dying)
			{
				respawn();
			}else
			{
				currentLevel = player.isExitingLevel;

				if(currentLevel == -2)
				{
					mapData.cleanup();
					loadLevel();

					return 1;
				}else
				{
					loadLevel();
				}

			}
		}
	}

#pragma region music
	
	waterPlayer.setVolume(mapData.getWaterPercentage(player.pos));
	greenPlayer.setVolume(mapData.getGreenPercentage(player.pos));
	redPlayer.setVolume(mapData.getRedPercentage(player.pos));
	grayPlayer.setVolume(mapData.getCavePercentage(player.pos));
	tikiPlayer.setVolume(mapData.getTikiPercentage(player.pos));


#pragma endregion

	//stencilRenderer2d.updateWindowMetrics(backGroundFBO.texture.GetSize().x, 
	//	backGroundFBO.texture.GetSize().y);

	//renderer2d.renderRectangle({ 100,100,100,100 }, Colors_Green);
	//renderer2d.flush();


	//renderer2d.currentCamera.position = { -500,-100 };

#pragma region controlls

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
		renderer2d.currentCamera.zoom -= deltaTime*2;
	}
	if (platform::isKeyHeld('X'))
	{
		renderer2d.currentCamera.zoom += deltaTime*2;
	}
	renderer2d.currentCamera.zoom = glm::clamp(renderer2d.currentCamera.zoom, 3.f, 7.f);


	if(jumpDelayTime>0)
	{
		jumpDelayTime -= deltaTime;
	}
	if (jumpDelayTime < 0) { jumpDelayTime = -1; }
	
	if ( player.isExitingLevel == -1 &&(input::isKeyPressedOn(input::Buttons::jump) || jumpDelayTime > 0))
	{
		if (input::isKeyPressedOn(input::Buttons::jump) && player.wallGrab == 0)
		{
			jumpDelayTime = 0.2;
		}

		if (player.wallGrab == 0)
		{
			if (player.canJump)
			{
				jumpDelayTime = 0;
				player.jump();
				player.canJump = 0;
				jumpParticle.set(player.pos, 0, player.movingRight);
			}
		}
		else if (player.wallGrab == 1)
		{
			jumpDelayTime = 0;
			player.strafe(-1);
			player.jumpFromWall();
			player.wallGrab = 0;
			player.redGrab = 0;
			player.grayGrab = 0;
			player.blueGrab = 0;
			jumpParticle.set(player.pos, 1, !player.movingRight);

		}
		else if (player.wallGrab == -1)
		{
			jumpDelayTime = 0;
			player.strafe(1);
			player.jumpFromWall();
			player.wallGrab = 0;
			player.redGrab = 0;
			player.grayGrab = 0;
			player.blueGrab = 0;
			jumpParticle.set(player.pos, 1, !player.movingRight);

		}
	}

	if (input::isKeyReleased(input::Buttons::jump))
	{
		if (player.velocity.y < 0)
		{
			player.velocity.y *= 0.4;
		}
	}

	if (input::isKeyPressedOn(input::Buttons::shoot) && currentArrow > -1 && !player.dying
		&& player.isExitingLevel == -1)
	{
		player.idleTime = 0;
		{

			for (auto& i : inventory)
			{
				if (i.type == actualInventorty[currentArrow].type)
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
	

	renderer2d.currentCamera.follow(player.pos + (player.dimensions / 2.f), deltaTime * 120, 30, renderer2d.windowW, renderer2d.windowH);
	//stencilRenderer2d.currentCamera = renderer2d.currentCamera;

	player.applyGravity(deltaTime);
	player.applyVelocity(deltaTime);

	player.resolveConstrains(mapData);

	player.updateMove();

	player.checkGrounded(mapData, deltaTime);

	if (input::isKeyHeld(input::Buttons::down))
	{
		player.wallGrab = 0;
	}
	else
	{
		player.checkWall(mapData, input::getMoveDir());
	}


	{
		static bool animateFall;
		static float animReloadTime;
		if (player.velocity.y >= 230)
		{
			if (animateFall)
			{
				jumpParticle.set(player.pos, 2, player.movingRight);
				animateFall = 0;
				animReloadTime = 0.2;
			}
			animReloadTime -= deltaTime;
			if (animReloadTime <= 0)
			{
				animateFall = 1;
				animReloadTime = 0.92;
			}
		}
		else
		{
			animateFall = 1;
		}
	}
	//mapRenderer.addBlock(renderer2d.toScreen({ 100,100,100,100 }), { 0,1,1,0 }, {1,1,1,1});
	//mapRenderer.render();

	mapData.clearColorData();

	simuleteLightSpot(player.pos + glm::vec2(player.dimensions.x / 2, player.dimensions.y / 2),
		playerLight, mapData, arrows, pickups, 0);

#pragma region inventory

	if (!platform::isFocused()) 
	{
		currentArrow = -1;
	}

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
		currentArrow = actualInventorty.size() - 1;
	}

	if (currentArrow >= actualInventorty.size())
	{
		currentArrow = -1;
	}

	if (actualInventorty.size() == 0)
	{
		currentArrow = -1;
	}

#pragma endregion

#pragma region lights

	bool isInRedBlock = 0;
	bool isInBlueBlock = 0;
	bool isInGrayBlock = 0;


	//the big for
	{
		int minX = 0;
		int minY = 0;
		int maxX = mapData.w;
		int maxY = mapData.h;

		minX = (player.pos.x) / BLOCK_SIZE;
		maxX = (player.pos.x + player.dimensions.x) / BLOCK_SIZE;

		minY = (player.pos.y - 2) / BLOCK_SIZE;
		maxY = (player.pos.y + player.dimensions.y - 1) / BLOCK_SIZE;

		minX = std::max(0, minX);
		minY = std::max(0, minY);
		maxX = std::min(mapData.w, maxX);
		maxY = std::min(mapData.h, maxY);

		static bool playedGrassSound = 0;
		bool playedGrassSoundThisFrame = 0;
		static float grassTimeDelay;

		player.iswebs = 0;
		for (int y = minY; y <= maxY; y++)
		{
			for (int x = minX; x <= maxX; x++)
			{
				auto& g = mapData.get(x, y);
				if (unLitTorch(g.type))
				{
					g.type++;
					wallLights.push_back({ { x,y }, mapData.getTorchLight(x,y) });
				}

				g.playerEntered = 1;

				if (g.type == Block::water3)
				{
					player.dying = 1;
				}else
				if (isSpike(g.type))
				{
					glm::vec2 playerP = player.pos;
					glm::vec2 blockP = { x*BLOCK_SIZE , y*BLOCK_SIZE };

					if(glm::distance(playerP, blockP) < (BLOCK_SIZE * 0.7))
					{
						player.dying = 1;
						player.lockMovementDie = 1;
					}
					
				}

				if (g.type == Block::flagDown)
				{
					if (mapData.get(playerSpawnPos.x, playerSpawnPos.y).type == Block::flagUp)
					{
						mapData.get(playerSpawnPos.x, playerSpawnPos.y).type = Block::flagDown;
					}

					saveState({ x,y }, currentLevel);

					playerSpawnPos = { x,y };
					g.type = Block::flagUp;
				}

				if (isRedNoSolid(g.type))
				{
					isInRedBlock = true;
				}
				if (isBlueNoSolid(g.type))
				{
					isInBlueBlock = true;
				}
				if (g.type == Block::fenceNoSolid)
				{
					isInGrayBlock = true;
				}

				if (g.type == Block::webBlock)
				{
					player.iswebs = true;
				}

				if (isInteractableGrass(g.type))
				{
					playedGrassSoundThisFrame = 1;
					if (soundPlayer.getStatus() == sf::Sound::Status::Stopped && !playedGrassSound && grassTimeDelay <=0)
					{
						grassTimeDelay = rand()%5+1;
						soundPlayer.setBuffer(leavesSoundbuffer);
						playedGrassSound = 1;
						soundPlayer.play();
					}
				}

				if(isSign(g.type))
				{
					auto iter = std::find_if(mapData.signDataVector.begin(), mapData.signDataVector.end(), 
						[x, y](signData &d)->bool {return (d.pos.x == x && d.pos.y == y); });

					if(iter!=mapData.signDataVector.end())
					{
						renderer2d.renderText({ BLOCK_SIZE*(x), BLOCK_SIZE*(y - 1) },
							iter->text.c_str(), font, { 1,1,1,1 }, 0.09);
					}
				}

				if (g.type == Block::levelExit)
				{

					auto iter = std::find_if(mapData.exitDataVector.begin(), mapData.exitDataVector.end(),
						[x, y](exitData &d) {return (d.pos.x == x && d.pos.y == y); });

					if (iter != mapData.exitDataVector.end())
					{
						if (input::isKeyPressedOn(input::Buttons::up))
						{
							player.isExitingLevel = iter->levelId;	
						}
					}else
					{
						player.isExitingLevel = -2;
					}
					
					renderer2d.renderText({ BLOCK_SIZE*(x), BLOCK_SIZE*(y - 1) },
						"Press Up to exit", font, { 1,1,1,1 }, 0.09);
				}

			}
		}

		grassTimeDelay -= deltaTime;
		if (grassTimeDelay < 0) { grassTimeDelay = 0; }

		if(!playedGrassSoundThisFrame)
		{
			playedGrassSound = 0;
		}

	}

	for (auto& i : wallLights)
	{
		float r = i.intensity;

		if (i.animationDuration <= 0)
		{
			r = i.intensity;
		}
		else
		{
			float perc;

			if (i.animationDuration > i.animationStartTime / 2.f)
			{
				perc = i.animationDuration - (i.animationStartTime / 2.f);
				perc = perc / (i.animationStartTime / 2.f);
				perc = 1 - perc;
				r *= perc * 1.2;
				r = std::max(r, 0.3f);
			}
			else
			{
				perc = i.animationStartTime / 2.f - i.animationDuration;
				perc = perc / (i.animationStartTime / 2.f);
				perc = 1 - perc;
				r *= (perc + 1);
			}

			i.animationDuration -= deltaTime;
		}

		//todo remove intensity
		simuleteLightSpot({ i.pos.x * BLOCK_SIZE + BLOCK_SIZE / 2,i.pos.y * BLOCK_SIZE + BLOCK_SIZE / 2 },
			r, mapData, arrows, pickups, 0);

	}

	for (auto& i : arrows)
	{
		if (i.type == Arrow::fireArrow)
		{
			float r = 5;

			if (i.liveTime < 5)
			{
				r *= (i.liveTime / (float)5);

				if (i.liveTime < 1)
				{
					r = 0;
				}
			}

			if (r > 0)
			{
				simuleteLightSpot({ i.pos },
					r, mapData, arrows, pickups, 0.1);
			}

		}

	}

#pragma endregion

#pragma region target
	{
		if (currentArrow >= 0 && !player.dying && player.isExitingLevel == -1 && 
			currentArrow < actualInventorty.size() )
		{
			glm::vec4 color = { 1,1,1,1 };

			switch (actualInventorty[currentArrow].type)
			{
			case Arrow::normalArrow:
				color = { 0.8,0.7,0.7,1 };
				break;
			case Arrow::fireArrow:
				color = { 0.9,0.0,0.0,1 };
				break;
			case Arrow::slimeArrow:
				color = { 0.0,0.9,0.0,1 };
				break;
			case Arrow::keyArrow:
				color = { 0.4,0.4,0.4,1 };
				break;
			default:
				break;
			}

			float fine = 0.6 * BLOCK_SIZE;
			glm::vec2 pos = player.pos + glm::vec2(player.dimensions.x / 2, player.dimensions.y / 2);
			glm::vec2 dir = input::getShootDir({ w / 2,h / 2 });
			float dist = BLOCK_SIZE * playerLight;
			for (int i = fine; i < dist; i += fine)
			{
				pos += fine * dir;

				if (pos.x < 0
					|| pos.y < 0
					|| pos.x >(mapData.w) * BLOCK_SIZE
					|| pos.y >(mapData.h) * BLOCK_SIZE)
				{

				}
				else
				{
					if (isCollidable(mapData.get(pos.x / BLOCK_SIZE, pos.y / BLOCK_SIZE).type))
					{
						dist = i;
						break;
					}
				}

				renderer2d.renderRectangle({ pos, 1,1 }, color);
				color.w *= 0.9;
			}
		}

	}
#pragma endregion

#pragma region draw map
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

		//glUseProgram(mapRenderer.shader.id);
		//glUniform1i(glGetUniformLocation(mapRenderer.shader.id, "u_time"), clock());
		mapRenderer.drawFromMapData(renderer2d, mapData, deltaTime, animPos);
	}
#pragma endregion


#pragma region pickups
	for (auto& i : pickups)
	{
		i.draw(renderer2d, arrowSprite, deltaTime);
		i.light = 0;

		if (i.colidePlayer(player) && i.cullDown <= 0)
		{
			i.cullDown = arrowPickupCullDown;
			inventory[i.type].count = inventory[i.type].maxCount;
			if (soundPlayer.getStatus() == sf::Sound::Status::Stopped)
			{
				soundPlayer.setBuffer(pickupSoundbuffer);
				soundPlayer.play();
			}
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

#pragma region particles


	for (int i = 0; i < crackParticles.size(); i++)
	{

		crackParticles[i].duration -= deltaTime;

		if (crackParticles[i].duration <= 0)
		{
			crackParticles.erase(crackParticles.begin() + i);
			i--;

		}else
		{
			crackParticles[i].draw(renderer2d, deltaTime, crackTexture);
		}
	}

	jumpParticle.draw(renderer2d, deltaTime, particlesSprite);

#pragma endregion


	player.draw(renderer2d, deltaTime, characterSprite);

#pragma region arrows
	for (auto i = 0; i < arrows.size(); i++)
	{
		auto& a = arrows[i];
		if (a.leftMap(mapData.w, mapData.h) || a.timeOut(deltaTime))
		{
			arrows.erase(arrows.begin() + i);
			i--;
			continue;
		}

		a.move(deltaTime * BLOCK_SIZE);
		a.draw(renderer2d, arrowSprite);
		a.checkCollision(mapData, isInRedBlock, isInBlueBlock, isInGrayBlock);
		a.light = 0;


		if (!a.shownAnim)
		{
			if (a.stuckInWall)
			{
				a.shownAnim = 1;
			}

			int x, y;
			x = a.pos.x / BLOCK_SIZE;
			y = a.pos.y / BLOCK_SIZE;

			if (x >= 0 && y >= 0 && x <= mapData.w && y <= mapData.h)
			{
				mapData.get(x, y).playerEntered = 1;
			}

		}
	}

#pragma endregion

#pragma region ui

	{
		auto c = renderer2d.currentCamera;
		renderer2d.currentCamera.setDefault();

		Ui::Frame f({ 0,0, w,h });

		//ui
		{
			Ui::Frame cornerLeft(Ui::Box().xLeft(20).yBottom(-20).xDimensionPercentage(0.1f).yAspectRatio(0.5f)());

			int centerCount = 1;
			int leftCount = 1;
			int rightCount = 1;

			if (actualInventorty.size() != 0)
			{

				int left = currentArrow - 1;
				if (left <= -1)
				{
				}
				else
				{
					if (left < -1)
					{
						left = actualInventorty.size() - 1;
					}

					leftCount = actualInventorty[left].count;
					left = actualInventorty[left].type;
				}

				if (actualInventorty.size() == 1)
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
				if (right >= actualInventorty.size())
				{
					right = -1;
				}
				else
				{
					rightCount = actualInventorty[right].count;
					right = actualInventorty[right].type;
				}

				if (left > -1)
				{
					float dim = 0.2;
					for (int i = -1; i < leftCount - 1; i++)
					{
						renderer2d.renderRectangle(
							Ui::Box().xLeft(i * 8).yCenter().yDimensionPercentage(0.7f).xAspectRatio(1)
							, { 0.4 * dim,0.4 * dim,0.4 * dim,1 }
						, {}, 45, arrowSprite, gl2d::computeTextureAtlas(Arrow::lastArror, 1, left, 0));
						dim += 0.1;
					}
				}


				if (right > -1)
				{
					float dim = 0.2;
					for (int i = -1; i < rightCount - 1; i++)
					{
						renderer2d.renderRectangle(
							Ui::Box().xRight(i * 8).yCenter().yDimensionPercentage(0.7f).xAspectRatio(1)
							, { 0.4 * dim,0.4 * dim,0.4 * dim,1 }
						, {}, 45, arrowSprite, gl2d::computeTextureAtlas(Arrow::lastArror, 1, right, 0));
						dim += 0.1;
					}
				}

				if (center > -1)
				{
					float dim = 1 - (centerCount / 10.f);

					for (int i = -1; i < centerCount - 1; i++)
					{

						renderer2d.renderRectangle(
							Ui::Box().xCenter(i * 10).yCenter().yDimensionPercentage(0.9f).xAspectRatio(1),
							{ dim,dim,dim,1 },
							{}, 45, arrowSprite, gl2d::computeTextureAtlas(Arrow::lastArror, 1, center, 0));
						dim += 0.1;
					}

					//renderer2d.renderText(Ui::Box().xCenter().yBottom()(),
					//	"1/2", font, { 1,1,1,1 }, 0.5);

				}

			}

		}

		renderer2d.currentCamera = c;
	}

#pragma endregion

	renderer2d.flush();

	return true;

}

void closeGame()
{
	waterPlayer.stop();
	greenPlayer.stop();
	tikiPlayer.stop();
	redPlayer.stop();
	grayPlayer.stop();
	soundPlayer.stop();

	saveState(playerSpawnPos, currentLevel);

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

	//ImGui::Begin("image");
	//ImGui::Image((void*)(intptr_t)characterSprite.id,
	//	{ 120,120 });
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
