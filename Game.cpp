#include "EnginePCH.h"

Game::Game()
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

	return true;
}

void Game::RunLoop()
{
	while(!mShouldQuit)
	{
		//TODO: Run systems
	}
}
