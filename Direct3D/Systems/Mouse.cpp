#include "stdafx.h"
#include "Mouse.h"

Mouse* Mouse::instance = NULL;

Mouse::Mouse()
{
	handle = NULL;

	clickPosition = D3DXVECTOR3(0, 0, 0);

	ZeroMemory(&buttonStatus,	 sizeof(byte) * MAX_INPUT_MOUSE);
	ZeroMemory(&buttonOldStatus, sizeof(byte) * MAX_INPUT_MOUSE);
	ZeroMemory(&buttonMap,		 sizeof(byte) * MAX_INPUT_MOUSE);

	position    = D3DXVECTOR3(0, 0, 0);
	positionOld = D3DXVECTOR3(0, 0, 0);
	moveValue   = D3DXVECTOR3(0, 0, 0);

	timeDblClk = GetDoubleClickTime();
	memset(&startDblClk, GetTickCount(), sizeof(DWORD) * MAX_INPUT_MOUSE);
	ZeroMemory(&buttonCount, sizeof(int  ) * MAX_INPUT_MOUSE);

	DWORD tLine = 0;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &tLine, 0);
}


Mouse::~Mouse()
{
}

void Mouse::Update(void)
{
	memcpy(buttonOldStatus, buttonStatus, sizeof(buttonOldStatus));

	ZeroMemory(buttonStatus, sizeof(buttonStatus));
	ZeroMemory(buttonMap, sizeof(buttonMap));

	buttonStatus[MOUSE_LBUTTON] = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	buttonStatus[MOUSE_RBUTTON] = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	buttonStatus[MOUSE_HBUTTON] = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;

	for (DWORD i = 0; i < MAX_INPUT_MOUSE; i++)
	{
		int tOldStatus = buttonOldStatus[i];
		int tStatus = buttonStatus[i];

		if (tOldStatus == 0 && tStatus == 1)
			buttonMap[i] = BUTTON_INPUT_STATUS_DOWN;
		else if (tOldStatus == 1 && tStatus == 0)
			buttonMap[i] = BUTTON_INPUT_STATUS_UP;
		else if (tOldStatus == 1 && tStatus == 1)
			buttonMap[i] = BUTTON_INPUT_STATUS_PRESS;
		else
			buttonMap[i] = BUTTON_INPUT_STATUS_NONE;
	}

	POINT point;
	GetCursorPos(&point);			//윈도우 절대좌표
	ScreenToClient(handle, &point); //handle의 창 내부 상대좌표

	positionOld.x = position.x;
	positionOld.y = position.y;

	position.x = float(point.x);
	position.y = float(point.y);

	moveValue = position - positionOld;
	positionOld.z = position.z;

	//더블클릭 체크
	DWORD tButtonStatus = GetTickCount();
	for (DWORD i = 0; i < MAX_INPUT_MOUSE; i++)
	{
		//버튼을 누르는데
		if (buttonMap[i] == BUTTON_INPUT_STATUS_DOWN)
		{
			//이전에 클릭했었으면
			if (buttonCount[i] == 1)
			{
				//더블클릭 허용시간이 오버됐으면 0으로 초기화
				if ((tButtonStatus - startDblClk[i]) >= timeDblClk)
					buttonCount[i] = 0;
			}
			buttonCount[i]++;

			//이번에 클릭한게 첫 클릭이면 더블클릭 시작위치로 설정
			if (buttonCount[i] == 1)
				startDblClk[i] = tButtonStatus;
		}

		//버튼을 떼는데
		if (buttonMap[i] == BUTTON_INPUT_STATUS_UP)
		{
			//클릭했었으면
			if (buttonCount[i] == 1)
			{
				//더블 클릭 허용시간이 오버됐으면 0으로 초기화
				if ((tButtonStatus - startDblClk[i]) >= timeDblClk)
					buttonCount[i] = 0;
			}
			//더블클릭이면
			else if (buttonCount[i] == 2)
			{
				//더블클릭 으로 설정
				if ((tButtonStatus - startDblClk[i]) <= timeDblClk)
					buttonMap[i] = BUTTON_INPUT_STATUS_DBLCLK;

				buttonCount[i] = 0;
			}
		}
	}
}

LRESULT Mouse::InputProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDOWN || message == WM_MOUSEMOVE)
	{
		clickPosition.x = (float)LOWORD(lParam);
		clickPosition.y = (float)HIWORD(lParam);
	}

	if (message == WM_MOUSEWHEEL)
	{
		short tWheelValue = (short)HIWORD(wParam);

		positionOld.z = position.z;
		position.z += (float)tWheelValue;
	}

	return TRUE;
}
