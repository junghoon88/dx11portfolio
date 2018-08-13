#pragma once

#define MAX_INPUT_MOUSE 8
#define gMouse Mouse::Get()

#define MOUSE_LBUTTON 0
#define MOUSE_RBUTTON 1
#define MOUSE_HBUTTON 2

class Mouse
{
private:
	Mouse();
	~Mouse();

public:
	inline void SetHandle(HWND handle) { this->handle = handle; }
	inline static Mouse* Get(void)  { /*assert(instance != NULL);*/ return instance; }
	inline static void Create(void)	{ assert(instance == NULL); instance = new Mouse(); }
	inline static void Delete(void)	{ SAFE_DELETE(instance); }

	void Update(void);

	LRESULT InputProc(UINT message, WPARAM wParam, LPARAM lParam);

	inline D3DXVECTOR3 GetClickPosition(void) { return clickPosition; }


	inline bool IsDown(DWORD button)  { return buttonMap[button] == BUTTON_INPUT_STATUS_DOWN; }
	inline bool IsUp(DWORD button)    { return buttonMap[button] == BUTTON_INPUT_STATUS_UP; }
	inline bool IsPress(DWORD button) { return buttonMap[button] == BUTTON_INPUT_STATUS_PRESS; }
	inline D3DXVECTOR3 GetMoveValue(void) { return moveValue; }

private:
	static Mouse* instance;

	HWND handle;

	D3DXVECTOR3 clickPosition;

	byte buttonStatus[MAX_INPUT_MOUSE];
	byte buttonOldStatus[MAX_INPUT_MOUSE];
	byte buttonMap[MAX_INPUT_MOUSE];


	D3DXVECTOR3 position;
	D3DXVECTOR3 positionOld;
	D3DXVECTOR3 moveValue;

	DWORD timeDblClk;
	DWORD startDblClk[MAX_INPUT_MOUSE];
	int   buttonCount[MAX_INPUT_MOUSE];


	enum
	{
		MOUSE_ROTATION_NONE = 0,
		MOUSE_ROTATION_LEFT,
		MOUSE_ROTATION_RIGHT
	};
	enum
	{
		BUTTON_INPUT_STATUS_NONE = 0,
		BUTTON_INPUT_STATUS_DOWN,
		BUTTON_INPUT_STATUS_UP,
		BUTTON_INPUT_STATUS_PRESS,
		BUTTON_INPUT_STATUS_DBLCLK
	};
};

