#pragma once

#define MAX_INPUT_KEY 255

#define gKeyboard Keyboard::Get()

class Keyboard
{
private:
	Keyboard();
	~Keyboard();

public:
	static Keyboard* Get(void)	{ assert(instance != NULL); return instance; }
	static void Create(void)	{ assert(instance == NULL); instance = new Keyboard(); }
	static void Delete(void)	{ SAFE_DELETE(instance); }

	void Update(void);

	inline bool IsDown(DWORD key)  { return keyMap[key] == KEY_INPUT_STATUS_DOWN; }
	inline bool IsUp(DWORD key)    { return keyMap[key] == KEY_INPUT_STATUS_UP; }
	inline bool IsPress(DWORD key) { return keyMap[key] == KEY_INPUT_STATUS_PRESS; }

	inline bool IsDown(DWORD statekey, DWORD key) 
	{
		return (keyMap[statekey] == KEY_INPUT_STATUS_PRESS && keyMap[key] == KEY_INPUT_STATUS_DOWN);
	}

private:
	static Keyboard* instance;

	byte keyState[MAX_INPUT_KEY];
	byte keyOldState[MAX_INPUT_KEY];
	byte keyMap[MAX_INPUT_KEY];

	enum
	{
		KEY_INPUT_STATUS_NONE = 0,
		KEY_INPUT_STATUS_DOWN,
		KEY_INPUT_STATUS_UP,
		KEY_INPUT_STATUS_PRESS,
	};
};

