#pragma once
#ifdef DX11
#include <memory>
#include "../../WindowsHeaders.h"
#include "D3D11Types.h"
#include "D3D11GraphicsDevice.h"
#include "../IRenderer.h"
#include "D3D11ResourceManager.h"

struct MeshElement;

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	virtual ~D3D11Renderer();
	void InitFrameBuffer();
	void InitShaders();
	void DrawMeshElement(MeshElement& element) const;
	void UpdateGlobalConstants(FramePacket& packet) const;
	bool Init(int width, int height);
	void RenderFrame(FramePacket& packet);
	D3D11ResourceManager* GetResourceManager() { return mResourceManager.get(); }

private:
	void Clear() const;
	void Present() const;

	std::unique_ptr<class D3D11GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<D3D11ResourceManager> mResourceManager;

	Matrix mProj;

	DepthStencilPtr mDepthBuffer;
	DepthStencilStatePtr mMeshDepthState;
	BlendStatePtr mMeshBlendState;

	//TODO: Properly implement multiple shaders
	PixelShaderPtr mPixelShader;
	VertexShaderPtr mVertexShader;
	SamplerStatePtr mDefaultSampler;
	GraphicsBufferPtr mCameraBuffer;

	SDL_Window* mWindow;
	char * mWindowName;

	int mWidth = 0;
	int mHeight = 0;
};
#endif

