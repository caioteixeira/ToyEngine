#include "Game.h"
#include "entityx/entityx.h"

Game::Game(): 
	mRenderer(*this)
	,mShouldQuit(false)
{
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

	if(!mRenderer.Init(1024, 768))
	{
		SDL_Log("Failed to initialized SDL.");
	}

	StartGame();

	return true;
}

void Game::RunLoop()
{
	while(!mShouldQuit)
	{
		ProcessInput();
		//TODO: Run systems
		mRenderer.RenderFrame();
	}
}

void Game::StartGame()
{
	//TODO: Level Loader
	entityx::EntityX * world = new entityx::EntityX();

	world->systems.add<RenderingSystem>();
	world->systems.configure();
	world->systems.update_all(1.0f);
	world->systems.update_all(0.5f);
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
