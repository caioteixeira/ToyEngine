#pragma once
#include <memory>
#include "WindowsHeaders.h"

class Renderer
{
public:
	Renderer(class Game& game);
	~Renderer();
	bool Init(int width, int height);

private:
	std::unique_ptr<class D3D11GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	class Game& mGame;
	int mWidth = 0;
	int mHeight = 0;
};

