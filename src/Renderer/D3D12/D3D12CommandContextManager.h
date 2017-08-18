#pragma once

#if DX12
#include "../../Math.h"

class D3D12GraphicsDevice;

class CommandContextManager
{
	friend class D3D12CommandContext;
public:
	CommandContextManager(D3D12GraphicsDevice* device);
	~CommandContextManager();

	D3D12CommandQueue& GetGraphicsQueue() { return mGraphicsQueue; }

	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFence(uint64_t fenceValue);
	void IdleGPU();

	void CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator);

	D3D12CommandContext * AllocateContext();
	void FreeContext(D3D12CommandContext* context);
	void DestroyAllContexts();

private:
	D3D12CommandQueue mGraphicsQueue;
	D3D12GraphicsDevice* mDevice;

	std::vector<std::unique_ptr<D3D12CommandContext>> mContextPool;
	std::queue<D3D12CommandContext *> mAvailableContexts;
	std::mutex mContextAllocationMutex;
};

#endif

