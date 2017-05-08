#pragma once
#ifdef DX12

#include "../../WindowsHeaders.h"
#include <memory>
#include "D3D12Device.h"

class Renderer
{
public:
	Renderer(class Game& game);
	~Renderer();
	bool Init(int width, int height);
	void RenderFrame();
private:
	void Clear();
	void Present();

	std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	class Game& mGame;
	int mWidth = 0;
	int mHeight = 0;
};

#endif
