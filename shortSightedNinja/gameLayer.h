#pragma once
#pragma once
#include <glm/vec2.hpp>
#include <unordered_map>
#include "DialogInteraction.h"
#include "Settings.h"

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

int isLMouseButtonReleased();

int isLMouseHeld();
int isRMouseHeld();

void showMouse(bool show);
bool isFocused();
bool mouseMoved();

bool writeEntireFile(const char* name, void* buffer, size_t size);
bool readEntireFile(const char* name, void* buffer, size_t size);
};

// game functions
bool initGame();
bool gameLogic(float deltaTime);
void closeGame();
void imguiFunc(float deltaTime);

void saveState(glm::ivec2 playerSpawnPos, int levelId, std::unordered_map<glm::ivec2, FullDialogData> &dialogData, int blueChanged, int redChanged, int grayChanged);
bool loadLevelFromLastState(int &level, glm::ivec2 &spawn, glm::ivec2 *dialogs, int& blueChanged, int& redChanged, int& grayChanged);
