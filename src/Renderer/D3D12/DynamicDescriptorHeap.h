#pragma once
#if DX12
#include <deque>
#include "d3dx12.h"
#include "../../WindowsHeaders.h"

/// A simple Dynamic Descriptor Heap implementation
class D3D12GraphicsDevice;

struct DescriptorAllocation
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle;
};

class DynamicDescriptorHeap
{
public:
	DynamicDescriptorHeap(D3D12GraphicsDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t maxSize = 1024);
	~DynamicDescriptorHeap() = default;

	DynamicDescriptorHeap(const DynamicDescriptorHeap& other) = delete;
	DynamicDescriptorHeap& operator=(const DynamicDescriptorHeap& other) = delete;
	//TODO: Implement move operations
	DynamicDescriptorHeap(DynamicDescriptorHeap&& other) = delete;
	DynamicDescriptorHeap& operator=(DynamicDescriptorHeap&& other) = delete;

	DescriptorAllocation AllocateDescriptor();
	void FinishCurrentFrame(uint64_t frameNum);
	void ReleaseCompletedFrames(uint64_t numCompletedFrames);
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return mHeap.Get(); }

	size_t GetMaxSize() const { return mMaxSize; }
	size_t GetUsedSize() const { return mUsedSize; }
	bool IsFull() const { return mUsedSize == mMaxSize; }
	bool IsEmpty() const { return mUsedSize == 0; }

private:
	size_t GetAllocationOffset(size_t size);
	static const size_t mInvalidOffset = static_cast<size_t>(-1);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;

	typedef std::pair<uint64_t, size_t> OffsetPair;
	std::deque<OffsetPair> mCompletedFrameTails;
	size_t mHead = 0;
	size_t mTail = 0;
	size_t mMaxSize = 0;
	size_t mUsedSize = 0;

	UINT mDescriptorSize = 0;
};

#endif
