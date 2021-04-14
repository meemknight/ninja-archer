#pragma once
#include "glm/vec2.hpp"
#include "mapData.h"
#include "mapRenderer.h"
#include <algorithm>

#undef min
#undef max

struct Entity
{
	glm::vec2 pos;
	glm::vec2 lastPos;

	glm::vec2 dimensions;

	glm::vec2 velocity;

	bool redGrab=0;
	bool blueGrab=0;
	bool grayGrab=0;
	int wallGrab = 0;
	int iceGrab = 0;
	int lastWallGrab = 0;

	bool grounded;
	bool movingRight;

	//this are related
	bool hasTouchGround;
	bool canJump;
	float timeLeftGrounded;

	float frameDuration = 0.10f;
	float idlefFameDuration = 0.13f;
	float currentCount = 0;
	int currentFrame = 0;

	float idleTime = 0;
	
	void resolveConstrains(MapData &mapData);

	float notGrabTime = 0;

	float accelerateTime = 0;
	float maxAccelerationTime = 0.15;

	bool moving = 0;
	bool dying = 0;
	bool lockMovementDie = 0;
	int isExitingLevel = -1;
	bool accelerating = 0;
	bool isSittingOnIce = 0;

	float getAcceleration();

	bool iswebs;

	void updateMove(float deltaTime);
	
	void strafe(int dir);

	void run(float speed, float deltaTime);

	void airRun(float speed);

	void applyGravity(float deltaTime);

	void applyVelocity(float deltaTime);

	void checkGrounded(MapData &mapDat, float deltaTime);

	void checkWall(MapData &mapData, int move);

	void jump();

	void draw(gl2d::Renderer2D &renderer, float deltaTime, gl2d::Texture characterSprite);

	void jumpFromWall();

private:
	void checkCollisionBrute(glm::vec2& pos, glm::vec2 lastPos, MapData& mapData,
		bool& upTouch, bool& downTouch, bool& leftTouch, bool& rightTouch);
	glm::vec2 performCollision(MapData &mapDat, glm::vec2 pos, glm::vec2 size, glm::vec2 delta,
		bool &upTouch, bool &downTouch, bool &leftTouch, bool &rightTouch);

};

bool aabb(glm::vec4 b1, glm::vec4 b2);

struct Arrow
{
	enum ArrowTypes: int
	{
		normalArrow=0,
		fireArrow,
		slimeArrow,
		keyArrow,
		bombArrow,
		lastArror,
	}type = normalArrow;


	//pos is the tip of the arrrow
	glm::vec2 pos;
	glm::vec2 lastPos;
	glm::vec2 shootDir;

	void draw(gl2d::Renderer2D &renderer, gl2d::Texture t);

	void move(float deltaTime);

	void checkCollision(MapData &mapData, bool redTouch, bool blueTouch, bool grayTouch, int& redChanged, int& blueChanged, int& grayChanged,
		glm::ivec2 litTorches[], int litTorchesCount);
	
	bool leftMap(int w, int h);

	bool timeOut(float deltaTime);

	float light = 0;

	float liveTime = 5;

	bool hitOnce = 0;
	bool stuckInWall = 0;
	bool shownAnim = 0;
};

struct Pickup
{
	Pickup() {};
	Pickup(int x, int y, int type) { pos.x = x, pos.y = y, this->type = type; };

	float animPos = 0;

	float light = 1;

	int type = 1;

	float cullDown = 0;

	glm::ivec2 pos;

	bool colidePlayer(Entity &player);

	void draw(gl2d::Renderer2D &renderer2d, gl2d::Texture arrowTexture, float deltaTime);

};

//gets a position diagonal of the player
static glm::vec2 getDiagonalBirdPos(glm::vec2 position, glm::vec2 playerPos)
{

	if(playerPos.x < position.x) // go right
	{
		return position - glm::vec2{ -9 * BLOCK_SIZE, 7 * BLOCK_SIZE };
	}else // go left
	{
		return position - glm::vec2{ +9 * BLOCK_SIZE, 7 * BLOCK_SIZE };
	}

}

constexpr float birdFrameDuration = 0.10f;

struct Bird
{
	glm::vec2 position;

	glm::vec2 destination;
	glm::vec2 startPos;

	//1 ender 2 leave
	int isMovingType = 0;

	void update(float deltaTime);

	void startMove(glm::vec2 start, glm::vec2 dest);

	void startEndMove(glm::vec2 start, glm::vec2 dest);

	void draw(gl2d::Renderer2D &renderer, float deltaTime, gl2d::Texture t, glm::vec2 playerPos);

	glm::ivec2 texturePos = {};

	float frameTime = 0;

	bool showing = 0;
	float changeTime = 0;

	float getShowPerc();

};

//todo calculate light for butterfly
struct Butterfly
{
	Butterfly() { create(); }
	Butterfly(glm::vec2 anchor):anchor(anchor), position(anchor) { create(); }

	glm::vec2 anchor;
	glm::vec2 position;

	glm::ivec2 texturePos = {0,0};
	float frameTime = 0;

	bool facingLeft = 0;
	void draw(gl2d::Renderer2D &renderer, float deltaTime, gl2d::Texture t);
	void create();

	float light = 0;

	glm::vec2 direction = {};
	float timeTillChangeDir = 0;
	void updateMove(float deltaTime, MapData &mapData);

};