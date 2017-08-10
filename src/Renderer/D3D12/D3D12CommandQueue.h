#pragma once
#if DX12

#include "D3D12Device.h"
#include "D3D12CommandAllocatorPool.h"

class D3D12CommandQueue
{
public:
	D3D12CommandQueue(D3D12_COMMAND_LIST_TYPE type, ID3D12Device * device);
	~D3D12CommandQueue();

	bool IsFenceComplete(uint64_t FenceValue);
	void WaitForFence(uint64_t fence);

	uint64_t ExecuteCommandList(ID3D12CommandList* List);
	uint64_t FinishCommandContext(D3D12CommandContext& context, bool waitForCompletion = false);
	ID3D12CommandAllocator * GetAllocator();
	void StoreAllocator(uint64_t fence, ID3D12CommandAllocator* allocator);
private:

	const D3D12_COMMAND_LIST_TYPE mType;

	ID3D12CommandQueue * mCommandQueue;
	ID3D12Device * mDevice;

	D3D12CommandAllocatorPool mAllocatorPool;
	std::mutex mFenceMutex;
	std::mutex mEventMutex;

	ID3D12Fence* mFence;
	uint64_t mNextFenceValue;
	uint64_t mLastCompletedFenceValue;
	HANDLE mFenceEventHandle;
};

#endif