#pragma once
#pragma once
#include <glm/vec2.hpp>

//todo remove
void errorMessage(const char *c);

///sets the mouse pos relative to the window's drawing area
void setRelMousePosition(int x, int y);


//gets the drawing region sizes
glm::ivec2 getWindowSize();

inline int getWindowSizeX() { return getWindowSize().x; }
inline int getWindowSizeY() { return getWindowSize().y; }

namespace platform
{

///gets the mouse pos relative to the window's drawing area
glm::ivec2 getRelMousePosition();

int isKeyHeld(int key);
int isKeyPressedOn(int key);

int isLMouseButtonPressed();
int isRMouseButtonPressed();

int isLMouseHeld();
int isRMouseHeld();

void showMouse(bool show);
bool isFocused();
bool mouseMoved();
};

// game functions
bool initGame();
bool gameLogic(float deltaTime);
void closeGame();
void imguiFunc(float deltaTime);