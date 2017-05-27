#pragma once
#include "WindowsHeaders.h"
#include "Renderer/Renderer.h"
#include "RenderingSystem.h"
#include "GameWorld.h"

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

	Engine::ECS::GameWorld mWorld;
	std::shared_ptr<Renderer> mRenderer;

	bool mShouldQuit;
};

