#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>

using namespace Engine;

Game::Game(): 
	mWorld()
	,mShouldQuit(false)
	,mInput(*this)
{
	mRenderer = std::make_shared<Renderer>();
}

Game::~Game()
{
}

bool Game::Init()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		SDL_Log("Failed to initialize SDL.");
		return false;
	}

	if(!mRenderer->Init(1024, 768))
	{
		SDL_Log("Failed to initialized Renderer.");
	}

	StartGame();

	return true;
}

void Game::RunLoop()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	while(!mShouldQuit)
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count() / 1000.0;
		currentTime = now;

		mInput.ProcessInput();
		//TODO: Run systems
		mWorld.Update(delta);
	}
}

void Game::StartGame()
{
	mWorld.Init(mRenderer);
	mWorld.LoadObjLevel("Assets/rungholt.obj");
}
