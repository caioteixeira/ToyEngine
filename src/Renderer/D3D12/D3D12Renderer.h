#pragma once
#if DX12

#include "../IRenderer.h"
#include "../../WindowsHeaders.h"
#include <memory>
#include "D3D12Device.h"

class D3D12Renderer : IRenderer
{
public:
	D3D12Renderer(class Game& game);
	D3D12Renderer();
	~D3D12Renderer();
	bool Init(int width, int height);
	void RenderFrame();
private:
	void Clear();
	void Present();

	std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	int mWidth = 0;
	int mHeight = 0;
};

#endif
