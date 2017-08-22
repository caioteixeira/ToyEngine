#pragma once
#if DX12

#include <vector>
#include "GPURingBuffer.h"

class D3D12GraphicsDevice;

class DynamicUploadHeap
{
public:
	DynamicUploadHeap(bool isCPUAcessible, class D3D12GraphicsDevice* device, size_t initialSize);

	DynamicUploadHeap(const DynamicUploadHeap&) = delete;
	DynamicUploadHeap(DynamicUploadHeap&&) = delete;
	DynamicUploadHeap& operator=(const DynamicUploadHeap&) = delete;
	DynamicUploadHeap& operator=(DynamicUploadHeap&&) = delete;
	
	DynamicAllocation Allocate(size_t size, size_t alignment = 256);

	void FinishFrame(uint64_t frameNum, uint64_t numCompletedFrames);

private:
	const bool mIsCPUAccessible;
	std::vector<GPURingBuffer> mRingBuffers;
	D3D12GraphicsDevice * mDevice = nullptr;
};

#endif
