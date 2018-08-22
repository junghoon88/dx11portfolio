#include "stdafx.h"
#include "./Systems/Window.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"


//새로운 콘솔창을 띄워서 printf, cout 으로 출력가능함
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

string	gStringParentDirectory = "";
wstring gWstringParentDirectory = L"";

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	//TODO : 프로젝트 경로 변경시 수정해야함
	{
		char l_pDir[_MAX_PATH];      // _MAX_PATH는 windows.h에 정의.
		GetCurrentDirectoryA(_MAX_PATH, l_pDir);
		string dir = string(l_pDir) + string("/");
		String::Replace(&dir, "\\", "/");
		//상위 폴더 2번
		dir = Path::GetParentDirectory(dir); 
		//dir = Path::GetParentDirectory(dir);
		gStringParentDirectory = dir;
		gWstringParentDirectory = String::ToWString(gStringParentDirectory);
	}



	D3DDesc desc;
	desc.Instance = instance;
	D3D::SetDesc(desc);

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SAFE_DELETE(window);

	srand((UINT)time(NULL));

	//return wParam;
	return 0;
}