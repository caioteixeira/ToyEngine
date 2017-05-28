#pragma once
#ifdef DX11
#include <memory>
#include "../../WindowsHeaders.h"
#include "D3D11Types.h"
#include "D3D11GraphicsDevice.h"
#include "../IRenderer.h"
#include "D3D11ResourceManager.h"

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	virtual ~D3D11Renderer();
	bool Init(int width, int height);
	void RenderFrame(FramePacket& packet);
	D3D11ResourceManager* GetResourceManager() { return mResourceManager.get(); }

private:
	void Clear() const;
	void Present() const;

	std::unique_ptr<class D3D11GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<D3D11ResourceManager> mResourceManager;
	SDL_Window* mWindow;
	int mWidth = 0;
	int mHeight = 0;
};
#endif

