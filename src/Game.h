#pragma once
#include "WindowsHeaders.h"
#include "Renderer/Renderer.h"
#include "RenderingSystem.h"

class Game
{
public:
	Game();
	~Game();
	bool Init();
	void RunLoop();

private:
	void StartGame();
	void ProcessInput();

	Renderer mRenderer;
	RenderingSystem mRenderSystem;

	bool mShouldQuit;
};

