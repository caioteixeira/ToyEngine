#if DX12
#include "D3D12CommandQueue.h"
#include <algorithm>

D3D12CommandQueue::D3D12CommandQueue(D3D12_COMMAND_LIST_TYPE type, ID3D12Device* device):
	mType(type), mDevice(device), mAllocatorPool(mType, device)
{
	//TODO: add asserts

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = mType;
	desc.NodeMask = 1;
	mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue));

	auto result = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	ThrowIfFailed(result, "Failed to create fence");
	mFence->Signal((uint64_t)mType << 56);

	mFenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
}

D3D12CommandQueue::~D3D12CommandQueue()
{
}

bool D3D12CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	// Avoid querying the fence value by testing against the last one seen.
	// The max() is to protect against an unlikely race condition that could cause the last
	// completed fence value to regress.
	if (fenceValue > mLastCompletedFenceValue)
	{
		mLastCompletedFenceValue = max(mLastCompletedFenceValue, mFence->GetCompletedValue());
	}

	return fenceValue <= mLastCompletedFenceValue;
}

void D3D12CommandQueue::WaitForFence(uint64_t fence)
{
	if (IsFenceComplete(fence))
		return;

	// TODO:  Think about how this might affect a multi-threaded situation.  Suppose thread A
	// wants to wait for fence 100, then thread B comes along and wants to wait for 99.  If
	// the fence can only have one event set on completion, then thread B has to wait for 
	// 100 before it knows 99 is ready.  Maybe insert sequential events?
	{
		std::lock_guard<std::mutex> LockGuard(mEventMutex);

		mFence->SetEventOnCompletion(fence, mFenceEventHandle);
		WaitForSingleObject(mFenceEventHandle, INFINITE);
		mLastCompletedFenceValue = fence;
	}
}

uint64_t D3D12CommandQueue::ExecuteCommandList(ID3D12CommandList* list)
{
	std::lock_guard<std::mutex> lock(mFenceMutex);

	auto hr = static_cast<ID3D12GraphicsCommandList*>(list)->Close();
	ThrowIfFailed(hr, "Failed to close command list!");

	mCommandQueue->ExecuteCommandLists(1, &list);

	mCommandQueue->Signal(mFence, mNextFenceValue);

	return mNextFenceValue++;
}

uint64_t D3D12CommandQueue::FinishCommandContext(D3D12CommandContext& context, bool waitForCompletion)
{
	//TODO: Flush resource barriers

	uint64_t fence = ExecuteCommandList(context.mCommandList);

	StoreAllocator(fence, context.mAllocator);
	context.mAllocator = nullptr;

	if(waitForCompletion)
	{
		WaitForFence(fence);
	}

	//TODO: FreeContext;

	return fence;
}

ID3D12CommandAllocator* D3D12CommandQueue::GetAllocator()
{
	return mAllocatorPool.GetAllocator(mFence->GetCompletedValue());
}

ID3D12CommandQueue * D3D12CommandQueue::GetCommandQueue()
{
	return mCommandQueue;
}

void D3D12CommandQueue::StoreAllocator(uint64_t fence, ID3D12CommandAllocator* allocator)
{
	mAllocatorPool.StoreAllocator(fence, allocator);
}

#endif
