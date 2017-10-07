#pragma once
#if DX11
#include "D3D11Types.h"

class D3D11GraphicsDevice;
class D3D11CommandContext
{
public:
	explicit D3D11CommandContext(D3D11GraphicsDevice* device);
	~D3D11CommandContext();

	//NonCopyable
	D3D11CommandContext() = default;
	D3D11CommandContext(const D3D11CommandContext&) = delete;
	D3D11CommandContext & operator=(const D3D11CommandContext&) = delete;

	void SetRenderTarget(RenderTargetPtr renderTarget) const;
	void SetDepthStencil(DepthStencilPtr depthStencil) const;
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

	void Draw(int vertexCount, int startVertexIndex) const;
	void DrawIndexed(int indexCount, int startIndexLocation, int baseLocation) const;

	void Finish();

private:
	D3D11GraphicsDevice* mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeferredContext = nullptr;
};

#endif
