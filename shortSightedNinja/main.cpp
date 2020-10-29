#include <Windows.h>
#include "gameLayer.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <ctime>
#include <stdio.h>
#include <algorithm>
#include <vector>
//#include <io.h>
//#include <fcntl.h>
#include "buildConfig.h"

#include "tools.h"
#include "opengl2Dlib.h"

#ifndef RemoveImgui
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

#endif

#include "input.h"
#include "Settings.h"

extern bool g_WantUpdateHasGamepad;

LRESULT CALLBACK windProc(HWND, UINT, WPARAM, LPARAM);
static bool quit = 0;

void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);

HWND wind;

static int lbutton = 0;
static int rbutton = 0;
static int lbuttonPressed = 0;
static int lbuttonReleased= 0;
static int rbuttonPressed = 0;
static int bMouseMoved = 0;

static bool isFocus = 0;

static bool fullScreen = false;

extern "C"
{
//	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
//	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

const float width = 960;
const float heigth = 680;

static LARGE_INTEGER queryFrequency;
static bool openglVsync = 0;

WINDOWPLACEMENT windowPlacementPrev = { sizeof(windowPlacementPrev) };


void setupFullscreen()
{

	DISPLAY_DEVICE displayDevice = {};
	displayDevice.cb = sizeof(displayDevice);
	bool foundPrimaryDevice = false;

	for (int i = 0; EnumDisplayDevices(nullptr, i, &displayDevice, EDD_GET_DEVICE_INTERFACE_NAME); i++)
	{
		if ((displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0)
		{
			foundPrimaryDevice = true;
			break;
		}

	}

	const char* monitorName = nullptr;

	if (foundPrimaryDevice)
	{
		monitorName = displayDevice.DeviceName;
	}

	DEVMODE monitorSettings = { };
	monitorSettings.dmSize = sizeof(monitorSettings);
	monitorSettings.dmDriverExtra = 0;
	bool found = 0;

	for (int i = 0; EnumDisplaySettings(monitorName, i, &monitorSettings); i++)
	{

		if (monitorSettings.dmDisplayFixedOutput == DMDFO_DEFAULT
			&& monitorSettings.dmPanningWidth == 1920 
			&& monitorSettings.dmPanningHeight == 1080
			)
		{
			found = 1;
			break;
		}

	}


	if (found) 
	{
		ChangeDisplaySettings(&monitorSettings, CDS_FULLSCREEN);
	}


}

LRESULT CALLBACK tempWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int MAIN
{

HINSTANCE h = GetModuleHandle(0);


#pragma region fake window
{
	//---- fake Window
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = tempWindProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "coco";
	wcex.hIconSm = NULL;
	
	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}
	
	HWND hwnd = CreateWindow(
		"coco",
		"dddd",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500,
		NULL,
		NULL,
		h,
		NULL
	);
	
	HDC hdc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd = {};
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	
	SetPixelFormat(hdc, nPixelFormat, &pfd);
	
	HGLRC hrc = wglCreateContext(hdc);
	
	wglMakeCurrent(hdc, hrc);
	
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
		return 1;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hrc);
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

}
#pragma endregion


	WNDCLASS wc = {};
	wc.hInstance = h;
	wc.lpfnWndProc = windProc;
	wc.lpszClassName = "wclass";
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClass(&wc);

	wind = CreateWindow(
		wc.lpszClassName,
		"Midnight Arrow ",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		heigth,
		NULL,
		NULL,
		h,
		0
	);

	HDC hdc;
	HGLRC hrc;

	EnableOpenGL(wind, &hdc, &hrc);
	//if (glewInit() != GLEW_OK)
	//{
	//	MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
	//	return 1;
	//}

	gl2d::setErrorFuncCallback([](const char* c) {elog(c); });
	gl2d::init();

#ifndef RemoveImgui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplWin32_Init(wind);
	const char* glslVersion = "#version 330";
	ImGui_ImplOpenGL3_Init(glslVersion);
	ImGui::StyleColorsDark();

#endif // !RemoveImgui

	input::loadXinput();


	if (!initGame())
	{
		return 0;
	}

	QueryPerformanceFrequency(&queryFrequency);

	LARGE_INTEGER time1 = {};
	LARGE_INTEGER time2 = {};

	QueryPerformanceCounter(&time1);
	QueryPerformanceCounter(&time2);

	if(gl2d::setVsync(true))
	{
		openglVsync = true;
	}

	while (!quit)
	{
		MSG msg;
		if (PeekMessage(&msg, wind, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			QueryPerformanceCounter(&time2);
			int deltaTime = time2.QuadPart - time1.QuadPart;
			QueryPerformanceCounter(&time1);

			double dDeltaTime = (double)deltaTime / (double)queryFrequency.QuadPart;
			static int count;
			static double accum;
			accum += dDeltaTime;
			count++;
		
			float fDeltaTime = std::min(dDeltaTime, 1.0 / 20.0);
		
			input::updateInput();

			if(input::isControllerInput())
			{
				platform::showMouse(false);
			}
			else 
			{
				platform::showMouse(true);
			}

			if (accum > 1)
			{
				accum -= 1;
				//ilog(count);
				count = 0;
			}

			if (!gameLogic(fDeltaTime))
			{
				quit = true;
			}

#ifndef RemoveImgui

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			imguiFunc(deltaTime);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

			SwapBuffers(hdc);
		
			if (!openglVsync) 
			{
				if(timeBeginPeriod(1) == TIMERR_NOERROR)
				{
					QueryPerformanceCounter(&time2);
					int deltaTime2 = time2.QuadPart - time1.QuadPart;
					double dDeltaTime2 = (double)deltaTime2 / (double)queryFrequency.QuadPart;
				
					int sleep = (1000.0 / 60.0) - (dDeltaTime2 * 1000.0);
					if (sleep > 0) { Sleep(sleep); }
					timeEndPeriod(1);
				}
				else 
				{
					int sleep = 0;
					do 
					{
						QueryPerformanceCounter(&time2);
						int deltaTime2 = time2.QuadPart - time1.QuadPart;
						double dDeltaTime2 = (double)deltaTime2 / (double)queryFrequency.QuadPart;

						sleep = (1000.0 / 60.0) - (dDeltaTime2 * 1000.0);
					}
					while (sleep > 0);
				}

				
			}

#pragma region fullScreen

			if (fullScreen != settings::isFullScreen()) 
			{
				DWORD dwStyle = GetWindowLong(wind, GWL_STYLE);
				if (dwStyle & (WS_OVERLAPPEDWINDOW))
				{
					MONITORINFO mi = { sizeof(mi) };
					if (
						GetWindowPlacement(wind, &windowPlacementPrev) &&
						GetMonitorInfo(MonitorFromWindow(wind,
							//MONITOR_DEFAULTTONEAREST
							MONITOR_DEFAULTTOPRIMARY
							),&mi)
						) 
					{
						SetWindowLong(wind, GWL_STYLE,
							dwStyle & ~WS_OVERLAPPEDWINDOW);
						SetWindowPos(wind, HWND_TOP,
							mi.rcMonitor.left, mi.rcMonitor.top,
							mi.rcMonitor.right - mi.rcMonitor.left,
							mi.rcMonitor.bottom - mi.rcMonitor.top,
							SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
					}
					fullScreen = 1;

					setupFullscreen();
				}
				else 
				{
					SetWindowLong(wind, GWL_STYLE,
						dwStyle | (WS_OVERLAPPEDWINDOW));

					SetWindowPlacement(wind, &windowPlacementPrev);
					SetWindowPos(wind, NULL, 0, 0, 0, 0,
						SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
						SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
					fullScreen = 0;

					ChangeDisplaySettings(nullptr, 0);
				}
			}

#pragma endregion


			lbuttonPressed = false;
			rbuttonPressed = false;
			bMouseMoved = false;
			lbuttonReleased = false;
		}
	
	}

	CloseWindow(wind);

	closeGame();
	//todo not always workking

	return 0;
}

LRESULT CALLBACK windProc(HWND wind, UINT m, WPARAM wp, LPARAM lp)
{
#ifndef RemoveImgui
	if (ImGui::GetCurrentContext() == NULL)
		goto endImgui;
	
	{
		ImGuiIO& io = ImGui::GetIO();
		switch (m)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			int button = 0;
			if (m == WM_LBUTTONDOWN || m == WM_LBUTTONDBLCLK) { button = 0; }
			if (m == WM_RBUTTONDOWN || m == WM_RBUTTONDBLCLK) { button = 1; }
			if (m == WM_MBUTTONDOWN || m == WM_MBUTTONDBLCLK) { button = 2; }
			if (m == WM_XBUTTONDOWN || m == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wp) == XBUTTON1) ? 3 : 4; }
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(wind);
			io.MouseDown[button] = true;
			goto endImgui;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int button = 0;
			if (m == WM_LBUTTONUP) { button = 0; }
			if (m == WM_RBUTTONUP) { button = 1; }
			if (m == WM_MBUTTONUP) { button = 2; }
			if (m == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wp) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = false;
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == wind)
				::ReleaseCapture();
			goto endImgui;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wp) / (float)WHEEL_DELTA;
			goto endImgui;
		case WM_MOUSEHWHEEL:
			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wp) / (float)WHEEL_DELTA;
			goto endImgui;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wp < 256)
				io.KeysDown[wp] = 1;
			goto endImgui;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wp < 256)
				io.KeysDown[wp] = 0;
			goto endImgui;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			io.AddInputCharacter((unsigned int)wp);
			goto endImgui;
		case WM_SETCURSOR:
			if (LOWORD(lp) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
				goto endImgui;;
			goto endImgui;
		case WM_DEVICECHANGE:
			if ((UINT)wp == DBT_DEVNODES_CHANGED)
				g_WantUpdateHasGamepad = true;
			goto endImgui;
		}
	}
endImgui:
#endif

	LRESULT l = 0;

	switch (m)
	{
	case WM_LBUTTONDOWN:
		lbutton = true;
		lbuttonPressed = true;
		break;
	case WM_RBUTTONDOWN:
		rbutton = true;
		rbuttonPressed = true;
		break;
	case WM_RBUTTONUP:
		rbutton = false;
		break;
	case WM_LBUTTONUP:
		lbutton = false;
		lbuttonReleased = true;
		break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lp;
		lpMMI->ptMinTrackSize.x = width;
		lpMMI->ptMinTrackSize.y = heigth;
	}
		break;
	case WM_CLOSE:
		quit = true;
		break;
	case WM_DESTROY:
		quit = true;
		break;
	case WM_MOUSEMOVE:
		bMouseMoved = 1;
		break;
	case WM_ACTIVATE:
		if (wp == WA_ACTIVE)
		{
			isFocus = true;
			if (fullScreen) 
			{
				setupFullscreen();
			}
		}
		else if (wp == WA_INACTIVE)
		{
			isFocus = false;

			if(fullScreen)
			{
				ChangeDisplaySettings(0, 0);
			}
		}
		break;

	default:
		l = DefWindowProc(wind, m, wp, lp);
		break;
	}

	return l;
}


void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{

	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32; //todo look into this (24)
	pfd.cDepthBits = 16;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (
		true
		//wglewIsSupported("WGL_ARB_create_context") == 1
		)
	{
		

		*hDC = GetDC(hwnd);

		const int iPixelFormatAttribList[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0 // End of attributes list
		};
		int attributes[] = {
			//WGL_CONTEXT_MAJOR_VERSION_ARB, 3
			//, WGL_CONTEXT_MINOR_VERSION_ARB, 2
			//, WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		int nPixelFormat = 0;
		UINT iNumFormats = 0;

		wglChoosePixelFormatARB(*hDC, iPixelFormatAttribList, NULL, 1, &nPixelFormat, (UINT*)&iNumFormats);

		SetPixelFormat(*hDC, nPixelFormat, &pfd);

		*hRC = wglCreateContextAttribsARB(*hDC, 0, attributes);

		wglMakeCurrent(*hDC, *hRC);

	}else
	{
		

		int iFormat;

		/* get the device context (DC) */
		*hDC = GetDC(hwnd);

		iFormat = ChoosePixelFormat(*hDC, &pfd);

		SetPixelFormat(*hDC, iFormat, &pfd);

		/* create and enable the render context (RC) */
		*hRC = wglCreateContext(*hDC);

		wglMakeCurrent(*hDC, *hRC);
	}

	
}


//platform independent functions implementation
///////////////////////////////////////////////
void errorMessage(const char *c)
{
	MessageBoxA(0, c, "error", MB_ICONERROR);
}

///sets the mouse pos relative to the window's drawing area
void setRelMousePosition(int x, int y)
{
	POINT p = { x, y };
	WINDOWPLACEMENT wp;

	GetWindowPlacement(wind, &wp);
	p.x += wp.rcNormalPosition.left;
	p.y += wp.rcNormalPosition.top;

	SetCursorPos(p.x, p.y);
}

//gets the drawing region sizes
glm::ivec2 getWindowSize()
{
	RECT r;
	GetClientRect(wind, &r);
	glm::ivec2 d;
	d.x = r.right - r.left;
	d.y = r.bottom - r.top;
	return d;
}

namespace platform
{
	///gets the mouse pos relative to the window's drawing area
	glm::ivec2 getRelMousePosition()
	{
		//todo refactor

		POINT p = {};
		GetCursorPos(&p);

		//WINDOWPLACEMENT wp;
		//
		//GetWindowPlacement(wind, &wp);
		//
		//p.x -= wp.rcNormalPosition.left;
		//p.y -= wp.rcNormalPosition.top;

		ScreenToClient(wind, &p);

		return { p.x, p.y };
	}

	int isKeyHeld(int key)
	{
		return GetAsyncKeyState(key) && platform::isFocused();
	}

	int isKeyPressedOn(int key)
	{
		return ( GetAsyncKeyState(key) & 0x8000) && platform::isFocused();
	}

	int isLMouseButtonPressed()
	{

#ifndef RemoveImgui
		ImGuiIO& io = ImGui::GetIO();
		return (!io.WantCaptureMouse) && lbuttonPressed && platform::isFocused();
#else
		return  lbuttonPressed && platform::isFocused();
#endif
	
	}

	int isLMouseButtonReleased()
	{
#ifndef RemoveImgui
		ImGuiIO& io = ImGui::GetIO();
		return (!io.WantCaptureMouse) && lbuttonReleased && platform::isFocused();
#else
		return  lbuttonReleased && platform::isFocused();
#endif

	}

	int isLMouseHeld()
	{
#ifndef RemoveImgui
		ImGuiIO& io = ImGui::GetIO();
		return (!io.WantCaptureMouse) && lbutton && platform::isFocused();
#else
		return lbutton && platform::isFocused();
#endif
	
	}

	int isRMouseButtonPressed()
	{
		return rbuttonPressed && platform::isFocused();
	}


	int isRMouseHeld()
	{
		return rbutton && platform::isFocused();
	}

	static bool lastShow = 1;
	void showMouse(bool show)
	{
		if(lastShow != show)
		{
			ShowCursor(show);

			if (show)
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				SendMessage(wind, WM_SETCURSOR, (WPARAM)wind, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
				//SetCursor(LoadCursor(GetModuleHandle(0), IDC_HAND));
			}
		}
	
		lastShow = show;
	}

	//todo probably rework
	bool isFocused()
	{
		//return 1;
		return GetActiveWindow() == wind;
	}

	bool mouseMoved()
	{
		return bMouseMoved;
	}


	bool writeEntireFile(const char* name, void* buffer, size_t size)
	{
		HANDLE file = CreateFile(name, GENERIC_WRITE, NULL, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (file == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		DWORD sizeWritten = 0;
		int rez = 1;

		if (!WriteFile(file, buffer, size, &sizeWritten, NULL))
		{
			rez = 0;
		}

		assert(size == sizeWritten);

		CloseHandle(file);

		return rez;
	}


	bool readEntireFile(const char* name, void* buffer, size_t size)
	{
		HANDLE file = CreateFile(name, GENERIC_READ, NULL, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (file == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		DWORD sizeRead = 0;

		int rez = 1;

		if (!ReadFile(file, buffer, size, &sizeRead, NULL))
		{
			rez = 0;
		}

		//if(sizeRead != size)
		//{
		//	rez = 0;
		//}

		CloseHandle(file);

		return rez;
	}

};
