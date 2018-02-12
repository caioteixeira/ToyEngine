#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>
#include <easy/profiler.h>
#include "EngineCore.h"

using namespace Engine;

Game::Game(): 
	mWorld()
	,mInput(*this)
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
		Logger::DebugLog("Failed to initialize SDL.");
		return false;
	}

	if(!mRenderer->Init(1440, 900))
	{
		Logger::DebugLog("Failed to initialized Renderer.");
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

		mInput.ProcessInput();
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
