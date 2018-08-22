#include "stdafx.h"
#include "./Systems/Window.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"


//���ο� �ܼ�â�� ����� printf, cout ���� ��°�����
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

string	gStringParentDirectory = "";
wstring gWstringParentDirectory = L"";

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	//TODO : ������Ʈ ��� ����� �����ؾ���
	{
		char l_pDir[_MAX_PATH];      // _MAX_PATH�� windows.h�� ����.
		GetCurrentDirectoryA(_MAX_PATH, l_pDir);
		string dir = string(l_pDir) + string("/");
		String::Replace(&dir, "\\", "/");
		//���� ���� 2��
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