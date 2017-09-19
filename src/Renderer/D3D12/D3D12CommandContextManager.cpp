#if DX12

#include "D3D12Device.h"
#include "D3D12CommandContextManager.h"

CommandContextManager::CommandContextManager(D3D12GraphicsDevice* device)
	: mDevice(device), mGraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT, device)
{
}

CommandContextManager::~CommandContextManager()
{
}

bool CommandContextManager::IsFenceComplete(uint64_t fenceValue)
{
	return mGraphicsQueue.IsFenceComplete(fenceValue);
}

void CommandContextManager::WaitForFence(uint64_t fenceValue)
{
	mGraphicsQueue.WaitForFence(fenceValue);
}

void CommandContextManager::IdleGPU()
{
	mGraphicsQueue.WaitForIdle();
}

void CommandContextManager::CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList ** list, ID3D12CommandAllocator ** allocator)
{
	auto device = mDevice->GetDevice();
	auto result = device->CreateCommandAllocator(type,
		IID_PPV_ARGS(allocator));
	ThrowIfFailed(result, "ERROR: Failed to create command allocator!");

	//TODO: Use Copy queue
	*allocator = mGraphicsQueue.GetAllocator();

	result = device->CreateCommandList(1, type, *allocator,
		nullptr, IID_PPV_ARGS(list));
	ThrowIfFailed(result, "ERROR: Failed to create command list!");
	(*list)->SetName(L"CommandList");
}

D3D12CommandContext * CommandContextManager::AllocateContext()
{
	std::lock_guard<std::mutex> lockGuard(mContextAllocationMutex);
	
	D3D12CommandContext* ret = nullptr;

	if (mAvailableContexts.empty())
	{
		ret = new D3D12CommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT, mDevice);
		mContextPool.emplace_back(ret);
	}
	else
	{
		ret = mAvailableContexts.front();
		mAvailableContexts.pop();
		ret->Reset();
	}
	

	assert(ret != nullptr);

	return ret;
}

void CommandContextManager::FreeContext(D3D12CommandContext * context)
{
	assert(context != nullptr);

	std::lock_guard<std::mutex> lockGuard(mContextAllocationMutex);
	mAvailableContexts.push(context);
}

void CommandContextManager::DestroyAllContexts()
{
	mContextPool.clear();
}

#endif