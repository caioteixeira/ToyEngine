#if DX12
#include "D3D12CommandContext.h"
#include "D3D12Device.h"

D3D12CommandContext::D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device)
	: mDevice(device)
{
	mDevice->CreateCommandList(type, &mCommandList, &mAllocator);
}

D3D12CommandContext::~D3D12CommandContext()
{
}

#endif