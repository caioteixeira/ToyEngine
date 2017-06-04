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

	bool CompileShaderFromFile(const char* inFileName, const char* szEntryPoint, const char* szShaderModel, std::vector<char>& outCompiledShaderCode) const;
	VertexShaderPtr CreateVertexShader(const std::vector<char>& compiledShaderCode) const;
	PixelShaderPtr CreatePixelShader(const std::vector<char>& compiledShaderCode) const;
	InputLayoutPtr CreateInputLayout(const InputLayoutElement* elements, int numElements, const std::vector<char>& compiledVertexShader) const;
	GraphicsBufferPtr CreateGraphicsBuffer(const void* rawData, int rawDataSize, EBindflags bindFlags, ECPUAccessFlags cpuAccessFlags, EGraphicsBufferUsage usage) const;
	GraphicsTexturePtr CreateTextureFromFile(const char* inFileName, int& outWidth, int& outHeight) const;
	DepthStencilPtr CreateDepthStencil() const;

	RasterizerStatePtr CreateRasterizerState(EFillMode inFillMode) const;
	DepthStencilStatePtr CreateDepthStencilState(bool inDepthTestEnable, EComparisonFunc inDepthComparisonFunction) const;
	BlendStatePtr CreateBlendState(bool inEnableBlend) const;
	SamplerStatePtr CreateSamplerState() const;

	void SetRenderTarget(RenderTargetPtr renderTarget);
	void SetDepthStencil(DepthStencilPtr depthStencil);
	void SetViewPort(float x, float y, float width, float height) const;
	void SetPrimitiveTopology(EPrimitiveTopology topology) const;

	void SetDepthStencilState(DepthStencilStatePtr inDepthStencilState) const;
	void SetRasterizerState(RasterizerStatePtr inRasterizerStatePtr) const;
	void SetBlendState(BlendStatePtr inBlendState) const;
	void SetPSSamplerState(SamplerStatePtr inSamplerState, int inStartSlot) const;
	void SetInputLayout(InputLayoutPtr inLayout) const;
	void SetVertexBuffer(GraphicsBufferPtr inBuffer, uint32_t inVertexSize) const;
	void SetIndexBuffer(GraphicsBufferPtr inBuffer) const;
	void SetVertexShader(VertexShaderPtr inVertexShader) const;
	void SetVSConstantBuffer(GraphicsBufferPtr inBuffer, int inStartSlot) const;
	void SetPixelShader(PixelShaderPtr inPixelShader) const;
	void SetPSTexture(GraphicsTexturePtr inTexture, int inStartSlot) const;
	void SetPSConstantBuffer(GraphicsBufferPtr inBuffer, int inStartSlot) const;

	void* MapBuffer(GraphicsBufferPtr buffer) const;
	void UnmapBuffer(GraphicsBufferPtr buffer) const;

	void ClearBackBuffer(const Vector3& inColor, float inAlpha) const;
	void ClearRenderTarget(RenderTargetPtr inRenderTarget, const Vector3& inColor, float inAlpha) const;
	void ClearDepthStencil(DepthStencilPtr inDepthStencil, float inDepth) const;
	void Draw(int vertexCount, int startVertexIndex) const;
	void DrawIndexed(int indexCount, int startIndexLocation, int baseLocation) const;
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
