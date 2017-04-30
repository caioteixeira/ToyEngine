#ifdef DX11
#include "D3D11Renderer.h"

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

	if(!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	mGraphicsDevice = std::make_unique<D3D11GraphicsDevice>(GetActiveWindow());

	return true;
}

void Renderer::RenderFrame()
{
	Clear();
	//TODO: Draw
	Present();
}

void Renderer::Clear()
{
	mGraphicsDevice->ClearBackBuffer(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
}

void Renderer::Present()
{
	mGraphicsDevice->Present();
}
#endif
