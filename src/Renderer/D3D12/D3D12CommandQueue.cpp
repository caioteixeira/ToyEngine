#if DX12
#include "D3D12CommandQueue.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include <functional>

D3D12CommandQueue::D3D12CommandQueue(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device) :
	mType(type), mDevice(device->GetDevice()), mAllocatorPool(mType, mDevice)
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
	{
		return;
	}

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
	auto hr = static_cast<ID3D12GraphicsCommandList*>(list)->Close();
	ThrowIfFailed(hr, "Failed to close command list!");

	std::lock_guard<std::mutex> lock(mFenceMutex);

	mCommandQueue->ExecuteCommandLists(1, &list);

	mCommandQueue->Signal(mFence, mNextFenceValue);
	SDL_Log("Signaled %d", mNextFenceValue);

	return mNextFenceValue++;
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
