#pragma once
#include "../ConstantBufferStructs.h"
#if DX12

#include "../IRenderer.h"
#include "../../WindowsHeaders.h"
#include <memory>
#include "D3D12Device.h"
#include "D3D12ResourceManager.h"
#include "../../EngineCore.h"

class D3D12Renderer : IRenderer
{
public:
	D3D12Renderer();
	~D3D12Renderer();
	bool Init(int width, int height);
	void RenderFrame(FramePacket & framePacket);
	D3D12ResourceManager* GetResourceManager() const;
private:
	void Clear() const;
	void Present() const;

	std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<D3D12ResourceManager> mResourceManager;

	char * mWindowName;
	SDL_Window* mWindow;
	int mWidth = 0;
	int mHeight = 0;

	Matrix mProj;

	static const int NUM_THREADS = 4;
	Engine::Core::WorkerPool mThreadPool;
};

#endif
