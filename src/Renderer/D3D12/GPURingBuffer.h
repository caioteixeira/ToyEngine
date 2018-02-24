#pragma once

#include <utility>
#include <cstdint>
#include <deque>
#include "../../WindowsHeaders.h"

struct DynamicAllocation
{
    DynamicAllocation(ID3D12Resource* buffer, size_t offset, size_t size) :
        buffer(buffer), Offset(offset), Size(size)
    {
    }

    ID3D12Resource* buffer = nullptr;
    size_t Offset = 0;
    size_t Size = 0;
    void* CPUAddress = 0;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = 0;
};

class GPURingBuffer
{
public:
    typedef std::pair<uint64_t, size_t> FrameNumOffsetPair;
    static const size_t invalidOffset = static_cast<size_t>(-1);

    GPURingBuffer(size_t maxSize, ID3D12Device* device, bool allowCPUAccess);

    ~GPURingBuffer();

    GPURingBuffer(GPURingBuffer&& rhs) noexcept;
    GPURingBuffer& operator =(GPURingBuffer&& rhs) noexcept;
    GPURingBuffer(const GPURingBuffer&) = delete;
    GPURingBuffer& operator =(const GPURingBuffer&) = delete;

    DynamicAllocation Allocate(size_t size);

    void FinishCurrentFrame(uint64_t frameNum);
    void ReleaseCompletedFrames(uint64_t numCompletedFrames);

    size_t GetMaxSize() const { return mMaxSize; }
    size_t GetUsedSize() const { return mUsedSize; }
    bool IsFull() const { return mUsedSize == mMaxSize; }
    bool IsEmpty() const { return mUsedSize == 0; }

private:
    size_t GetAllocationOffset(size_t);

    void Destroy();

    void* mCPUVirtualAddress;
    D3D12_GPU_VIRTUAL_ADDRESS mGPUVirtualAddress;
    Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;

    std::deque<FrameNumOffsetPair> mCompletedFrameTails;
    size_t mHead = 0;
    size_t mTail = 0;
    size_t mMaxSize = 0;
    size_t mUsedSize = 0;
};
