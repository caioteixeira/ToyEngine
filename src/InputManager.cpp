#include "InputManager.h"
#include "WindowsHeaders.h"
#include "Game.h"
#include "Core/imgui/imgui.h"
#include "Core/imgui/imgui_impl_dx12.h"

InputManager::InputManager(Game& game)
	: mGame(game)
{
}

InputManager::~InputManager()
{
}

void InputManager::ProcessInput()
{
	MSG msg = { nullptr };
	while (PeekMessage(&msg, mMainWindow, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			mGame.QuitGame();
			return;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void InputManager::SetMainWindow(HWND hwnd)
{
	mMainWindow = hwnd;
}

LRESULT InputManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX12_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	return 0;
}

void InputManager::Initialize()
{
	//TODO: Implement actions and stuff
}
