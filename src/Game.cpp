#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>
#include <easy/profiler.h>

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

	if(!mRenderer->Init(1440, 900))
	{
		SDL_Log("Failed to initialized Renderer.");
	}

	StartGame();

	return true;
}

void Game::RunLoop()
{
	int numFrames = 0;
	float executionTime = 10.0f;
	float remainingTime = executionTime;

	EASY_PROFILER_ENABLE
	EASY_MAIN_THREAD;
	profiler::startListen();

	auto lastFrame = std::chrono::high_resolution_clock::now();
	while(!mShouldQuit && remainingTime > 0.0f)
	{
		EASY_BLOCK("GameLoop")

		auto now = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - lastFrame).count() / 1000.0f;
		lastFrame = now;

		remainingTime -= delta;
		numFrames++;

		mInput.ProcessInput();
		//TODO: Run systems
		mWorld.Update(delta);

		EASY_END_BLOCK;
	}

	SDL_Log("%f FPS", numFrames / executionTime);

	profiler::dumpBlocksToFile("test_profile.prof");
}

void Game::StartGame()
{
	mWorld.Init(mRenderer);
	mWorld.LoadObjLevel("Assets/lost_empire.obj");
}
