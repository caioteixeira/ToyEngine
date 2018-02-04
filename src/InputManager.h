#pragma once
#include "WindowsHeaders.h"

class Game;

class InputManager
{
public:
	InputManager(Game& game);
	~InputManager();
	void ProcessInput();
	void SetMainWindow(HWND hwnd);
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Initialize();
private:
	Game& mGame;
	HWND mMainWindow;
};

