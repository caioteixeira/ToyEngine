#include "D3D12CommandAllocatorPool.h"

D3D12CommandAllocatorPool::D3D12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type, ID3D12Device* device) :
    mCommandListType(type), mDevice(device)
{
}

D3D12CommandAllocatorPool::~D3D12CommandAllocatorPool()
{
    for (auto i = 0; i < mAllocatorPool.size(); ++i)
    {
        mAllocatorPool[i]->Release();
    }

    mAllocatorPool.clear();
}

ID3D12CommandAllocator* D3D12CommandAllocatorPool::GetAllocator(uint64_t completedFenceValue)
{
    std::lock_guard<std::mutex> lock(mAllocatorMutex);

    ID3D12CommandAllocator* allocator = nullptr;

    if (!mFreeQueue.empty())
    {
        auto allocatorPair = mFreeQueue.front();

        if (allocatorPair.first <= completedFenceValue)
        {
            allocator = allocatorPair.second;
            ThrowIfFailed(allocator->Reset());
            mFreeQueue.pop();
        }
    }

    //No allocator were ready to be reused
    if (allocator == nullptr)
    {
        auto result = mDevice->CreateCommandAllocator(mCommandListType, IID_PPV_ARGS(&allocator));

        wchar_t name[32];
        swprintf(name, 32, L"Command Allocator %zu", mAllocatorPool.size());
        allocator->SetName(name);
        mAllocatorPool.push_back(allocator);
    }

    return allocator;
}

void D3D12CommandAllocatorPool::StoreAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator)
{
    std::lock_guard<std::mutex> lock(mAllocatorMutex);

    mFreeQueue.push(std::make_pair(fenceValue, allocator));
}
