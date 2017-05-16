#ifdef DX12
#include "D3D12Renderer.h"

D3D12Renderer::D3D12Renderer(): mWindow(nullptr)
{
}


D3D12Renderer::~D3D12Renderer()
{
}

bool D3D12Renderer::Init(int width, int height)
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

void D3D12Renderer::RenderFrame()
{
	//TODO: WIP Clear is clearing and swapping buffers
	Clear();
}

void D3D12Renderer::Clear()
{
	mGraphicsDevice->ClearBackBuffer(Vector3::Zero, 1.0f);
}

void D3D12Renderer::Present()
{

}
#endif
