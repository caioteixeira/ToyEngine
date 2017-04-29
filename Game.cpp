#include "Game.h"

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

	return true;
}

void Game::RunLoop()
{
	while(!mShouldQuit)
	{
		//TODO: Run systems
	}
}

void Game::Quit()
{
}

void Game::StartGame()
{

}
