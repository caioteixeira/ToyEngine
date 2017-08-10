#if DX12
#include "DynamicUploadHeap.h"

DynamicUploadHeap::DynamicUploadHeap(bool isCPUAcessible, D3D12GraphicsDevice* device, size_t initialSize)
	: mIsCPUAccessible(isCPUAcessible), mDevice(device)
{
	mRingBuffers.emplace_back(initialSize, device->GetDevice(), mIsCPUAccessible);
}

DynamicAllocation DynamicUploadHeap::Allocate(size_t size, size_t alignment)
{
	const size_t alignmentMask = alignment - 1;

	//TODO: Assert if alignment is power of two

	const size_t alignedSize = (size + alignmentMask) & ~alignmentMask;

	auto allocation = mRingBuffers.back().Allocate(alignedSize);
	if(!allocation.buffer)
	{
		auto newMaxSize = mRingBuffers.back().GetMaxSize() * 2;
		while(newMaxSize < size)
		{
			newMaxSize *= 2;
		}

		mRingBuffers.emplace_back(newMaxSize, mDevice->GetDevice(), mIsCPUAccessible);
		allocation = mRingBuffers.back().Allocate(alignedSize);
	}

	return allocation;
}

void DynamicUploadHeap::FinishFrame(uint64_t frameNum, uint64_t numCompletedFrames)
{
	size_t numBuffersToDelete = 0;

	for(size_t i = 0; i < mRingBuffers.size(); ++i)
	{
		auto & ringBuffer = mRingBuffers[i];
		ringBuffer.FinishCurrentFrame(frameNum);
		ringBuffer.ReleaseCompletedFrames(numCompletedFrames);
		if(numBuffersToDelete == i && i < mRingBuffers.size() - 1 && ringBuffer.IsEmpty())
		{
			++numBuffersToDelete;
		}
	}

	if(numBuffersToDelete)
	{
		mRingBuffers.erase(mRingBuffers.begin(), mRingBuffers.begin() + numBuffersToDelete);
	}
}

#endif