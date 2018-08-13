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
	GetCursorPos(&point);			//������ ������ǥ
	ScreenToClient(handle, &point); //handle�� â ���� �����ǥ

	positionOld.x = position.x;
	positionOld.y = position.y;

	position.x = float(point.x);
	position.y = float(point.y);

	moveValue = position - positionOld;
	positionOld.z = position.z;

	//����Ŭ�� üũ
	DWORD tButtonStatus = GetTickCount();
	for (DWORD i = 0; i < MAX_INPUT_MOUSE; i++)
	{
		//��ư�� �����µ�
		if (buttonMap[i] == BUTTON_INPUT_STATUS_DOWN)
		{
			//������ Ŭ���߾�����
			if (buttonCount[i] == 1)
			{
				//����Ŭ�� ���ð��� ���������� 0���� �ʱ�ȭ
				if ((tButtonStatus - startDblClk[i]) >= timeDblClk)
					buttonCount[i] = 0;
			}
			buttonCount[i]++;

			//�̹��� Ŭ���Ѱ� ù Ŭ���̸� ����Ŭ�� ������ġ�� ����
			if (buttonCount[i] == 1)
				startDblClk[i] = tButtonStatus;
		}

		//��ư�� ���µ�
		if (buttonMap[i] == BUTTON_INPUT_STATUS_UP)
		{
			//Ŭ���߾�����
			if (buttonCount[i] == 1)
			{
				//���� Ŭ�� ���ð��� ���������� 0���� �ʱ�ȭ
				if ((tButtonStatus - startDblClk[i]) >= timeDblClk)
					buttonCount[i] = 0;
			}
			//����Ŭ���̸�
			else if (buttonCount[i] == 2)
			{
				//����Ŭ�� ���� ����
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
