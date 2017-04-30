#ifdef DX12
#include "D3D12Renderer.h"

Renderer::Renderer(Game& game)
	:mWindow(nullptr)
	,mGame(game)
{
}


Renderer::~Renderer()
{
}

bool Renderer::Init(int width, int height)
{
	mWindow = SDL_CreateWindow("Toy Engine", 100, 100, width, height, 0);
	mWidth = width;
	mHeight = height;

	if (!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	return true;
}
#endif