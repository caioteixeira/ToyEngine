#pragma once
#include "WindowsHeaders.h"
#include "Renderer/Renderer.h"
#include "RenderingSystem.h"
#include "GameWorld.h"
#include "InputManager.h"

class Game
{
public:
	Game();
	~Game();
	bool Init();
	void RunLoop();
	void QuitGame() { mShouldQuit = true; }

private:
	void StartGame();

	Engine::ECS::GameWorld mWorld;
	std::shared_ptr<Renderer> mRenderer;
	InputManager mInput;

	bool mShouldQuit;
};

