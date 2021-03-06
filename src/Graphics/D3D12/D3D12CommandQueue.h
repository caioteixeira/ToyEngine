#pragma once
#include "D3D12Device.h"
#include "D3D12CommandAllocatorPool.h"

class D3D12CommandQueue
{
public:
    D3D12CommandQueue(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device);
    ~D3D12CommandQueue();

    bool IsFenceComplete(uint64_t FenceValue);
    void WaitForFence(uint64_t fence);
    void WaitForIdle() { WaitForFence(mNextFenceValue - 1); };

    uint64_t ExecuteCommandList(ID3D12CommandList* List);
    ID3D12CommandAllocator* GetAllocator();
    ID3D12CommandQueue* GetCommandQueue();
    void StoreAllocator(uint64_t fence, ID3D12CommandAllocator* allocator);
    void Flush();

    uint64_t GetLastCompletedFenceValue() { return mLastCompletedFenceValue; }
    uint64_t GetActualFenceValue() { return mFence->GetCompletedValue(); }
    uint64_t GetLastSignaledFenceValue() { return mNextFenceValue - 1; }

private:

    const D3D12_COMMAND_LIST_TYPE mType;

    ID3D12CommandQueue* mCommandQueue;
    ID3D12Device* mDevice;

    D3D12CommandAllocatorPool mAllocatorPool;
    std::mutex mFenceMutex;
    std::mutex mEventMutex;

    ID3D12Fence* mFence;
    uint64_t mNextFenceValue;
    uint64_t mLastCompletedFenceValue;
    HANDLE mFenceEventHandle;
};
