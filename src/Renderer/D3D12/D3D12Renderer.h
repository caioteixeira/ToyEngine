#pragma once
#include "../../WindowsHeaders.h"
#include <memory>

class Renderer
{
public:
	Renderer(class Game& game);
	~Renderer();
	bool Init(int width, int height);

private:
//	std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	class Game& mGame;
	int mWidth = 0;
	int mHeight = 0;
};

