#include "stdafx.h"
#include "Window.h"
#include "../Program.h"

Program* Window::program = NULL;

Window::Window()
{

	D3DDesc desc;
	D3D::GetDesc(&desc);

	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	ATOM wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen)
	{
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx(
		WS_EX_APPWINDOW												// DWORD dwExStyle,
		, desc.AppName.c_str()										// LPCWSTR lpClassName,
		, desc.AppName.c_str()										// LPCWSTR lpWindowName,
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW	// DWORD dwStyle,
		, CW_USEDEFAULT												// int X,
		, CW_USEDEFAULT												// int Y,
		, CW_USEDEFAULT												// int nWidth,
		, CW_USEDEFAULT												// int nHeight,
		, NULL														// HWND hWndParent,
		, (HMENU)NULL												// HMENU hMenu,
		, desc.Instance												// HINSTANCE hInstance,
		, NULL														// LPVOID lpParam);
	);
	assert(desc.Handle != NULL);

	D3D::SetDesc(desc);

	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width)  / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow(desc.Handle, centerX, centerY, rect.right - rect.left, rect.bottom - rect.top, TRUE);

	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle); //Brings the thread that created the specified window into the foreground and activates the window. Keyboard input is directed to the window, and various visual cues are changed for the user. The system assigns a slightly higher priority to the thread that created the foreground window than it does to other threads.
	SetFocus(desc.Handle);

	ShowCursor(true);
}

Window::~Window()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (desc.bFullScreen)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

WPARAM Window::Run(void)
{
	MSG msg;

	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3D::Create();
	Keyboard::Create();
	Mouse::Create();
	CommandManager::Create();
	LightManager::Create();

	gMouse->SetHandle(desc.Handle);


	Time::Create();
	gTime->Start();

	ImGui::Create(desc.Handle, gDevice, gDC);
	ImGui::StyleColorsDark();

	program = new Program();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gTime->Update(60);

			if (ImGui::IsMouseHoveringAnyWindow() == FALSE
				&& ImGui::IsAnyItemActive() == FALSE)
			{
				gKeyboard->Update();
				gMouse->Update();
			}

			program->Update();
			ImGui::Update();

			program->PreRender();
			D3D::Get()->Clear();
			{
				program->Render();
				program->PostRender();
				ImGui::Render();
			}
			D3D::Get()->Present();
		}
	}
	SAFE_DELETE(program);

	ImGui::Delete();

	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();
	D3D::Delete();
	CommandManager::Delete();
	LightManager::Delete();

	return msg.wParam;
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::WndProc((UINT*)handle, message, wParam, lParam))
		return true;

	if(gMouse) gMouse->InputProc(message, wParam, lParam);

	if (message == WM_SIZE)
	{
		ImGui::Invalidate();
		{
			if (program != NULL)
			{
				float width = (float)LOWORD(lParam);
				float height = (float)HIWORD(lParam);

				if (D3D::Get()) D3D::Get()->ResizeScreen(width, height);

				program->ResizeScreen();
			}
		}
		ImGui::Validate();
	}


	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}
	
	//180410 :: DefWindowProc 안해주면 CreateWindow 결과값이 NULL이 나옴;
	return DefWindowProc(handle, message, wParam, lParam);
}
