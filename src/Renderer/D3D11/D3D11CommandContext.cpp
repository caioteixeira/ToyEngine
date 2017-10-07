#if DX11
#include "D3D11CommandContext.h"
#include "D3D11GraphicsDevice.h"


D3D11CommandContext::D3D11CommandContext(D3D11GraphicsDevice* device)
	: mDevice(device)
{
	auto d3dDevice = mDevice->GetDevice();
	auto hr = d3dDevice->CreateDeferredContext(0, mDeferredContext.GetAddressOf());
	ThrowIfFailed(hr);
}

D3D11CommandContext::~D3D11CommandContext()
{
}

void D3D11CommandContext::SetRenderTarget(RenderTargetPtr renderTarget) const
{
	auto depthStencil = mDevice->GetDepthStencilBuffer();
	auto rt = renderTarget.get();
	mDeferredContext->OMSetRenderTargets(1, &rt, depthStencil.get());
}

void D3D11CommandContext::SetDepthStencil(DepthStencilPtr depthStencil) const
{
	auto renderTarget = mDevice->GetBackBufferRenderTarget().get();
	mDeferredContext->OMSetRenderTargets(1, &renderTarget, depthStencil.get());
}

void D3D11CommandContext::SetViewPort(float x, float y, float width, float height) const
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeferredContext->RSSetViewports(1, &viewport);
}

void D3D11CommandContext::SetPrimitiveTopology(EPrimitiveTopology topology) const
{
	mDeferredContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
}

void D3D11CommandContext::SetDepthStencilState(DepthStencilStatePtr inDepthStencilState) const
{
	mDeferredContext->OMSetDepthStencilState(inDepthStencilState.get(), 1);
}

void D3D11CommandContext::SetRasterizerState(RasterizerStatePtr rasterizerStatePtr) const
{
	mDeferredContext->RSSetState(rasterizerStatePtr.get());
}

void D3D11CommandContext::SetBlendState(BlendStatePtr inBlendState) const
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	mDeferredContext->OMSetBlendState(inBlendState.get(), blendFactor, 0xffffffff);
}

void D3D11CommandContext::SetPSSamplerState(SamplerStatePtr inSamplerState, int inStartSlot) const
{
	auto sampler = inSamplerState.get();
	mDeferredContext->PSSetSamplers(inStartSlot, 1, &sampler);
}

void D3D11CommandContext::SetInputLayout(InputLayoutPtr inLayout) const
{
	mDeferredContext->IASetInputLayout(inLayout.get());
}

void D3D11CommandContext::SetVertexBuffer(GraphicsBufferPtr inBuffer, uint32_t inVertexSize) const
{
	auto buffer = inBuffer.get();
	uint32_t offset = 0;
	mDeferredContext->IASetVertexBuffers(0, 1, &buffer, &inVertexSize, &offset);
}

void D3D11CommandContext::SetIndexBuffer(GraphicsBufferPtr inBuffer) const
{
	mDeferredContext->IASetIndexBuffer(inBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
}

void D3D11CommandContext::SetVertexShader(VertexShaderPtr inVertexShader) const
{
	mDeferredContext->VSSetShader(inVertexShader.get(), nullptr, 0);
}

void D3D11CommandContext::SetVSConstantBuffer(GraphicsBufferPtr inBuffer, int inStartSlot) const
{
	auto buffer = inBuffer.get();
	mDeferredContext->VSSetConstantBuffers(inStartSlot, 1, &buffer);
}

void D3D11CommandContext::SetPixelShader(PixelShaderPtr inPixelShader) const
{
	mDeferredContext->PSSetShader(inPixelShader.get(), nullptr, 0);
}

void D3D11CommandContext::SetPSTexture(GraphicsTexturePtr inTexture, int inStartSlot) const
{
	auto texture = inTexture.get();
	mDeferredContext->PSSetShaderResources(inStartSlot, 1, &texture);
}

void D3D11CommandContext::SetPSConstantBuffer(GraphicsBufferPtr inBuffer, int inStartSlot) const
{
	auto buffer = inBuffer.get();
	mDeferredContext->PSSetConstantBuffers(inStartSlot, 1, &buffer);
}

void* D3D11CommandContext::MapBuffer(GraphicsBufferPtr buffer) const
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	mDeferredContext->Map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	return mapped.pData;
}

void D3D11CommandContext::UnmapBuffer(GraphicsBufferPtr buffer) const
{
	mDeferredContext->Unmap(buffer.get(), 0);
}

void D3D11CommandContext::Draw(int vertexCount, int startVertexIndex) const
{
	mDeferredContext->Draw(vertexCount, startVertexIndex);
}

void D3D11CommandContext::DrawIndexed(int indexCount, int startIndexLocation, int baseLocation) const
{
	mDeferredContext->DrawIndexed(indexCount, startIndexLocation, baseLocation);
}

void D3D11CommandContext::Finish()
{
	mDevice->ExecuteDeferredContext(mDeferredContext.Get());
	mDevice->FreeContext(this);
}

#endif
