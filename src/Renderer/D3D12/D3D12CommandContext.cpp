#if DX12
#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandContextManager.h"

D3D12CommandContext::D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device)
	: mDevice(device)
{
	mContextManager = mDevice->GetCommandContextManager();
	mContextManager->CreateCommandList(type, &mCommandList, &mAllocator);
}

D3D12CommandContext::~D3D12CommandContext()
{
}

uint64_t D3D12CommandContext::Finish(bool waitCompletion)
{
	assert(mAllocator != nullptr);

	auto& queue = mContextManager->GetGraphicsQueue();

	uint64_t fenceValue = queue.ExecuteCommandList(mCommandList);
	queue.StoreAllocator(fenceValue, mAllocator);
	mAllocator = nullptr;

	if (waitCompletion)
	{
		mContextManager->WaitForFence(fenceValue);
	}

	mContextManager->FreeContext(this);

	return fenceValue;
}

void D3D12CommandContext::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE & rtvHandle, Color& clearColor)
{
	float color[4];
	color[0] = clearColor.x;
	color[1] = clearColor.y;
	color[2] = clearColor.z;
	color[3] = 1.0f;

	mCommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
}

void D3D12CommandContext::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE & dsvHandle, float depth, UINT8 stencil)
{
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

void D3D12CommandContext::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& rtv, D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	mCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);
}

void D3D12CommandContext::TransitionResource(ID3D12Resource * resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	assert(stateBefore != stateAfter);

	D3D12_RESOURCE_BARRIER barrierDesc;

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = stateBefore;
	barrierDesc.Transition.StateAfter = stateAfter;

	//TODO: Verify if a transition already started
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	//TODO: Cache multiple barriers and delay submission
	mCommandList->ResourceBarrier(1, &barrierDesc);

}

void D3D12CommandContext::Reset()
{
	assert(mCommandList != nullptr && mAllocator == nullptr);
	mAllocator = mContextManager->GetGraphicsQueue().GetAllocator();
	mCommandList->Reset(mAllocator, nullptr);
}

#endif

void D3D12CommandContext::SetViewport(const D3D12_VIEWPORT & viewport)
{
	mCommandList->RSSetViewports(1, &viewport);
}

void D3D12CommandContext::SetScissor(const D3D12_RECT & rect)
{
	assert(rect.left < rect.right && rect.top < rect.bottom);
	mCommandList->RSSetScissorRects(1, &rect);
}
