#pragma once
#include <vector>
#ifdef DX11
#include "D3D11Types.h"
#include "../../WindowsHeaders.h"
#include "../../Math.h"

class D3D11GraphicsDevice
{
public:
	D3D11GraphicsDevice(void *inWindow);
	~D3D11GraphicsDevice();

	RenderTargetPtr GetBackBufferRenderTarget() const
	{
		return mBackBufferRenderTarget;
	}

	VertexShaderPtr CreateVertexShader(const std::vector<char>& compiledShaderCode);
	PixelShaderPtr CreatePixelShader(const std::vector<char>& compiledShaderCode);
	InputLayoutPtr CreateInputLayout(const InputLayoutElement* elements, int numElements, const std::vector<char>& compiledVertexShader) const;
	GraphicsBufferPtr CreateGraphicsBuffer(const void* rawData, int rawDataSize, EBindflags bindFlags, ECPUAccessFlags cpuAccessFlags, EGraphicsBufferUsage usage) const;

	RasterizerStatePtr CreateRasterizerState(EFillMode inFillMode) const;

	void SetRenderTarget(RenderTargetPtr renderTarget);
	void SetDepthStencil(DepthStencilPtr depthStencil);
	void SetViewPort(float x, float y, float width, float height) const;
	void SetPrimitiveTopology(EPrimitiveTopology topology) const;

	void SetDepthStencilState(DepthStencilStatePtr inDepthStencilState);
	void SetRasterizerState(RasterizerStatePtr inRasterizerStatePtr) const;
	void SetBlendState(BlendStatePtr inBlendState);

	void ClearBackBuffer(const Vector3& inColor, float inAlpha) const;
	void ClearRenderTarget(RenderTargetPtr inRenderTarget, const Vector3& inColor, float inAlpha) const;
	void Present() const;
private:
	void CreateDevice();
	HRESULT CreateSwapChain(UINT backBufferWidth, UINT backBufferHeight, HWND window);
	RenderTargetPtr CreateBackBufferRenderTargetView() const;

	RenderTargetPtr mBackBufferRenderTarget;
	RenderTargetPtr mCurrentRenderTarget;
	DepthStencilPtr mCurrentDepthStencil;

	ID3D11Device * mDevice = nullptr;
	ID3D11Device1 * mDevice1 = nullptr;
	ID3D11DeviceContext * mImmediateContext = nullptr;
	ID3D11DeviceContext1* mImmediateContext1 = nullptr;
	IDXGISwapChain * mSwapChain = nullptr;
	IDXGISwapChain1* mSwapChain1 = nullptr;

	UINT mWindowWidth;
	UINT mWindowHeight;
};
#endif
