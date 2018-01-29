#pragma once
#include "WindowsHeaders.h"

class Game;

class InputManager
{
public:
	InputManager(Game& game);
	~InputManager();
	LRESULT ProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Initialize();
private:
	Game& mGame;
};

