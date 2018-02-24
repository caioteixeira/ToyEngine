#pragma once

#include "D3D12CommandContext.h"
#include <vector>
#include <queue>
#include <mutex>

class D3D12CommandAllocatorPool
{
public:
    D3D12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type, ID3D12Device* device);
    ~D3D12CommandAllocatorPool();

    ID3D12CommandAllocator* GetAllocator(uint64_t completedFenceValue);
    void StoreAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

private:
    const D3D12_COMMAND_LIST_TYPE mCommandListType;

    ID3D12Device* mDevice;

    std::vector<ID3D12CommandAllocator *> mAllocatorPool;
    std::queue<std::pair<uint64_t, ID3D12CommandAllocator *>> mFreeQueue;
    std::mutex mAllocatorMutex;
};
