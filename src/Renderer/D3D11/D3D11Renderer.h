#pragma once
#include <memory>
#include "../../WindowsHeaders.h"
#include "D3D11Types.h"
#include "D3D11GraphicsDevice.h"

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

	std::unique_ptr<class D3D11GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	class Game& mGame;
	int mWidth = 0;
	int mHeight = 0;
};

