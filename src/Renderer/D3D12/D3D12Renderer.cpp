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

	mGraphicsDevice = std::make_unique<D3D12GraphicsDevice>(GetActiveWindow());

	return true;
}

void Renderer::RenderFrame()
{
	//TODO: WIP Clear is clearing and swapping buffers
	Clear();
}

void Renderer::Clear()
{
	mGraphicsDevice->ClearBackBuffer(Vector3::Zero, 1.0f);
}

void Renderer::Present()
{

}
#endif
