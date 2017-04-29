#pragma once
#include "WindowsHeaders.h"
#include "Renderer.h"

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

	class Renderer mRenderer;

	bool mShouldQuit;
};

