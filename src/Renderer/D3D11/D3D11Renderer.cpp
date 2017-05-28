#ifdef DX11
#include "D3D11Renderer.h"
#include "D3D11ResourceManager.h"

D3D11Renderer::D3D11Renderer()
	: mWindow(nullptr)
{
	
}

D3D11Renderer::~D3D11Renderer()
{
}

bool D3D11Renderer::Init(int width, int height)
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
	mResourceManager = std::make_unique<D3D11ResourceManager>(*mGraphicsDevice);

	return true;
}

void D3D11Renderer::RenderFrame(FramePacket& packet)
{
	Clear();
	//TODO: Draw
	Present();
}

void D3D11Renderer::Clear() const
{
	mGraphicsDevice->ClearBackBuffer(Vector3::Zero, 1.0f);
}

void D3D11Renderer::Present() const
{
	mGraphicsDevice->Present();
}
#endif

