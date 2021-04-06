#ifdef PLATFORM_GLFW_IMPL
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <stb_truetype/stb_truetype.h>
#include "gl2d/gl2d.h"
#include <iostream>
#include <ctime>
#include "platformTools.h"
#include "config.h"
#include <raudio.h>
#include "platformInput.h"
#include "gameLayer.h"
#include <fstream>
#include <chrono>
#include "input.h"
#include "Settings.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#undef min
#undef max
#include <GLFW/glfw3.h>

//export g1lobals
float fullScreenZoom = 1;
//

bool fullScreen = 0;


bool keyboardMousePressedFlag = 0;
bool platform::keyboardMousePressed()
{
	return keyboardMousePressedFlag;
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

	keyboardMousePressedFlag = 1;

	bool state = 0;

	if(action == GLFW_PRESS)
	{
		state = 1;
	}else if(action == GLFW_RELEASE)
	{
		state = 0;
	}else
	{
		return;
	}

	if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
	{
		int index = key - GLFW_KEY_A;
		platform::internal::setButtonState(platform::Button::A + index, state);
	}else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		int index = key - GLFW_KEY_0;
		platform::internal::setButtonState(platform::Button::NR0 + index, state);
	}else
	{
	//special keys
		//GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT

		if (key == GLFW_KEY_SPACE)
		{
			platform::internal::setButtonState(platform::Button::Space, state);
		}
		else
		if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
		{
			platform::internal::setButtonState(platform::Button::Enter, state);
		}
		else
		if (key == GLFW_KEY_ESCAPE)
		{
			platform::internal::setButtonState(platform::Button::Escape, state);
		}
		else
		if (key == GLFW_KEY_UP)
		{
			platform::internal::setButtonState(platform::Button::Up, state);
		}
		else
		if (key == GLFW_KEY_DOWN)
		{
			platform::internal::setButtonState(platform::Button::Down, state);
		}
		else
		if (key == GLFW_KEY_LEFT)
		{
			platform::internal::setButtonState(platform::Button::Left, state);
		}
		else
		if (key == GLFW_KEY_RIGHT)
		{
			platform::internal::setButtonState(platform::Button::Right, state);
		}
	
	}
	
};

void mouseCallback(GLFWwindow *window, int key, int action, int mods)
{
	keyboardMousePressedFlag = 1;

	bool state = 0;

	if (action == GLFW_PRESS)
	{
		state = 1;
	}
	else if (action == GLFW_RELEASE)
	{
		state = 0;
	}
	else
	{
		return;
	}

	if(key == GLFW_MOUSE_BUTTON_LEFT)
	{
		platform::internal::setLeftMouseState(state);
	}else
	if (key == GLFW_MOUSE_BUTTON_RIGHT)
	{
		platform::internal::setRightMouseState(state);
	}
	

}

bool windowFocus = 1;


void windowFocusCallback(GLFWwindow *window, int focused)
{
	if (focused)
	{
		windowFocus = 1;
	}
	else
	{
		windowFocus = 0;
		//if you not capture the release event when the window loses focus,
		//the buttons will stay pressed
		platform::internal::resetInputsToZero();
	}
}

void windowSizeCallback(GLFWwindow *window, int x, int y)
{
	platform::internal::resetInputsToZero();
}

int mouseMovedFlag = 0;

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	mouseMovedFlag = 1;
}

#pragma region platform functions

GLFWwindow *wind = 0;

namespace platform
{

	//todo test
	void setRelMousePosition(int x, int y)
	{
		glfwSetCursorPos(wind, x, y);

	}

	glm::ivec2 getRelMousePosition()
	{
		double x = 0, y = 0;
		glfwGetCursorPos(wind, &x, &y);

		return { x, y };
	}

	glm::ivec2 getWindowSize()
	{
		int x = 0; int y = 0;

		glfwGetWindowSize(wind, &x, &y);

		return { x, y };
	}

	void showMouse(bool show)
	{
		if(show)
		{
			glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}else
		{
			glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

	}

	bool isFocused()
	{
		return windowFocus;
	}

	bool mouseMoved()
	{
		return mouseMovedFlag;
	}

	bool writeEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ofstream f(name, std::ios::binary);

		if(!f.is_open())
		{
			return 0;
		}

		f.write((char*)buffer, size);

		f.close();

		return 1;
	}


	bool readEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ifstream f(name, std::ios::binary);

		if (!f.is_open())
		{
			return 0;
		}

		f.read((char *)buffer, size);

		f.close();

		return 1;
	}

};
#pragma endregion



int main()
{

#ifdef _WIN32
#ifdef _MSC_VER 
#if INTERNAL_BUILD
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();
#endif
#endif
#endif


#pragma region window and opengl

	permaAssertComment(glfwInit(), "err initializing glfw");
	glfwWindowHint(GLFW_SAMPLES, 4);

	int w = 900;
	int h = 600;
	
	//	WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
	//	WGL_SAMPLES_ARB, 4,
	glfwWindowHint(GLFW_SAMPLES, 4);

	wind = glfwCreateWindow(w, h, "geam", nullptr, nullptr);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);

	glfwSetKeyCallback(wind, keyCallback);
	glfwSetMouseButtonCallback(wind, mouseCallback);
	glfwSetWindowFocusCallback(wind, windowFocusCallback);
	glfwSetWindowSizeCallback(wind, windowSizeCallback);
	glfwSetCursorPosCallback(wind, cursorPositionCallback);
	
	
	permaAssertComment(gladLoadGL(), "err initializing glad");

	glEnable(GL_MULTISAMPLE);


#pragma endregion

#pragma region gl2d

	gl2d::init();
	gl2d::setErrorFuncCallback([](const char *c) { permaAssertComment(0, c); });

#pragma endregion

#pragma region audio
	InitAudioDevice();
#pragma endregion

#pragma region initGame
	if (!initGame())
	{
		return 0;
	}
#pragma endregion


	//long lastTime = clock();
	
	auto stop = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(wind))
	{

	#pragma region deltaTime

		//long newTime = clock();
		//float deltaTime = (float)(newTime - lastTime) / CLOCKS_PER_SEC;
		//lastTime = clock();
		auto start = std::chrono::high_resolution_clock::now();

		float deltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(start - stop)).count() / 1000000.0f;
		stop = std::chrono::high_resolution_clock::now();

		float augmentedDeltaTime = deltaTime;
		if (augmentedDeltaTime > 1.f / 10) { augmentedDeltaTime = 1.f / 10; }
	
	#pragma endregion

		input::updateInput();

	#pragma region game logic

		//if(active)

		if (!gameLogic(augmentedDeltaTime))
		{
			return 0;
		}

	#pragma endregion

	#pragma region fullscreen 

		if(platform::isFocused() && fullScreen != settings::isFullScreen())
		{
			static int lastW = w;
			static int lastH = w;
			static int lastPosX = 0;
			static int lastPosY = 0;

			if(settings::isFullScreen())
			{
				lastW = w;
				lastH = h;

				//glfwWindowHint(GLFW_DECORATED, NULL); // Remove the border and titlebar..  
				glfwGetWindowPos(wind, &lastPosX, &lastPosY);


				auto monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode *mode = glfwGetVideoMode(monitor);
				
				// switch to full screen
				glfwSetWindowMonitor(wind, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);


				fullScreen = 1;

			}else
			{
				//glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // 
				glfwSetWindowMonitor(wind, nullptr, lastPosX, lastPosY, lastW, lastH, 0);

				fullScreen = 0;
			}

		}

		//if(input::isControllerInput())
		//{
		//	platform::showMouse(false);
		//}else
		//{
		//	platform::showMouse(true);
		//}

	#pragma endregion



	#pragma region reset flags

		mouseMovedFlag = 0;
		keyboardMousePressedFlag = 0;
		platform::internal::updateAllButtons();

	#pragma endregion

	#pragma region window stuff

		glfwPollEvents();
		glfwSwapBuffers(wind);

	#pragma endregion

	}

	closeGame();

	//if you want the console to stay after closing the window
	//std::cin.clear();
	//std::cin.get();
}

#endif