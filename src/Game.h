#pragma once
#include "WindowsHeaders.h"
#include "Renderer/Renderer.h"

class Game
{
public:
	Game();
	~Game();
	bool Init();
	void RunLoop();
	void Quit();

private:
	void StartGame();
	void ProcessInput();

	class Renderer mRenderer;

	bool mShouldQuit;
};

