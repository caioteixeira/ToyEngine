#pragma once
#include "../IRenderer.h"
#ifdef DX11
#include <memory>
#include "../../WindowsHeaders.h"
#include "D3D11Types.h"
#include "D3D11GraphicsDevice.h"

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	virtual ~D3D11Renderer();
	bool Init(int width, int height);
	void RenderFrame(FramePacket& packet);

private:
	void Clear();
	void Present();

	std::unique_ptr<class D3D11GraphicsDevice> mGraphicsDevice;
	SDL_Window* mWindow;
	int mWidth = 0;
	int mHeight = 0;
};
#endif

