#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>
#include <easy/profiler.h>

using namespace Engine;

Game::Game(HINSTANCE appInst):
	mAppInst(appInst)
	,mWorld()
	,mInput(*this)
	,mShouldQuit(false)
{
	mRenderer = std::make_shared<Renderer>();
	mGame = this;
}

Game::~Game()
{
}

Game* Game::mGame = nullptr;
Game* Game::GetInstance()
{
	return mGame;
}

bool Game::Init()
{
	/*
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		SDL_Log("Failed to initialize SDL.");
		return false;
	}*/

	if (!InitMainWindow(1440, 900))
	{
		Logger::Log("Failed to initialize Main Window!");
		return false;
	}

	if(!mRenderer->Init(1440, 900))
	{
		Logger::Log("Failed to initialized Renderer.");
		return false;
	}

	StartGame();

	return true;
}

void Game::RunLoop()
{
	EASY_PROFILER_ENABLE
	EASY_MAIN_THREAD;
	profiler::startListen();

	auto lastFrame = std::chrono::high_resolution_clock::now();
	while(!mShouldQuit)
	{
		EASY_BLOCK("GameLoop")

		auto now = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - lastFrame).count() / 1000.0f;
		lastFrame = now;

		//TODO: Run systems
		mWorld.Update(delta);

		EASY_END_BLOCK;
	}

	profiler::dumpBlocksToFile("test_profile.prof");
}

void Game::StartGame()
{
	mWorld.Init(mRenderer);
	mWorld.LoadObjLevel("Assets/sponza.obj");
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Game::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(mInput.ProcessInput(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_SIZE:
		//TODO: Resize
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Game::InitMainWindow(int width, int height)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	width = R.right - R.left;
	height = R.bottom - R.top;

	mMainWnd = CreateWindow("MainWnd", "ToyEngine",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mAppInst, 0);
	if (!mMainWnd)
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mMainWnd, SW_SHOW);
	UpdateWindow(mMainWnd);

	return true;
}
