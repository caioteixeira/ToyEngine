#pragma once
#include "WindowsHeaders.h"
#include "Renderer/Renderer.h"
#include "GameWorld.h"
#include "InputManager.h"

class Game
{
public:
	Game(HINSTANCE appInst);
	~Game();
	
	bool Init();
	void RunLoop();
	void QuitGame() { mShouldQuit = true; }

	//TODO: I don't really like singletons, should find a better solution :/
	static Game* GetInstance();
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) const;

private:
	void StartGame();
	bool InitMainWindow(int width, int height);

	static Game* mGame;

	HINSTANCE mAppInst = nullptr; // application instance handle
	HWND      mMainWnd = nullptr; // main window handle

	Engine::ECS::GameWorld mWorld;
	std::shared_ptr<Renderer> mRenderer;
	InputManager mInput;

	bool mShouldQuit;
};

