#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>

using namespace Engine;

Game::Game(): 
	mWorld()
	,mShouldQuit(false)
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
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count();
		currentTime = now;

		ProcessInput();
		//TODO: Run systems
		mWorld.Update(delta);

		std::cout << "dt: " << delta << std::endl;
	}
}

void Game::StartGame()
{
	mWorld.Init(mRenderer);
	mWorld.LoadObjLevel("Assets/lost_empire.obj");
}

void Game::ProcessInput()
{
	//TODO: Move to an input manager
	// Poll events from SDL
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mShouldQuit = true;
			break;
		default:
			// Ignore other events
			break;
		}
	}
}
