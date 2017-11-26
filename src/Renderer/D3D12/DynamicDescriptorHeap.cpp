#include "DynamicDescriptorHeap.h"
#include "D3D12Device.h"

DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12GraphicsDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t maxSize)
	: mMaxSize(maxSize)
{
	mHeap = device->CreateDescriptorHeap(type, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, static_cast<int>(maxSize));
	mDescriptorSize = device->GetDescriptorSize(type);
}

DescriptorAllocation DynamicDescriptorHeap::AllocateDescriptor()
{
	const auto offset = GetAllocationOffset(1);
	
	DescriptorAllocation alloc = {};

	if(offset == mInvalidOffset)
	{
		SDL_Log("Dynamic descriptor Heap is full!", SDL_LOG_CATEGORY_ERROR);
		return alloc;
	}
	
	alloc.CPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mHeap->GetCPUDescriptorHandleForHeapStart());
	alloc.CPUHandle.Offset(offset, mDescriptorSize);

	alloc.GPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mHeap->GetGPUDescriptorHandleForHeapStart());
	alloc.GPUHandle.Offset(offset, mDescriptorSize);

	return alloc;
}

void DynamicDescriptorHeap::FinishCurrentFrame(uint64_t frameNum)
{
	mCompletedFrameTails.push_back(std::make_pair(frameNum, mTail));
}

void DynamicDescriptorHeap::ReleaseCompletedFrames(uint64_t numCompletedFrames)
{
	while (!mCompletedFrameTails.empty() && mCompletedFrameTails.front().first < numCompletedFrames)
	{
		auto &oldestFrameTail = mCompletedFrameTails.front().second;
		const auto completed = mCompletedFrameTails.front().first;
		if (mUsedSize > 0)
		{
			if (oldestFrameTail > mHead)
			{
				mUsedSize -= oldestFrameTail - mHead;
			}
			else
			{
				mUsedSize -= (mMaxSize - mHead);
				mUsedSize -= oldestFrameTail;
			}
		}
		mHead = oldestFrameTail;
		mCompletedFrameTails.pop_front();
	}
}

inline size_t DynamicDescriptorHeap::GetAllocationOffset(size_t size = 1)
{
	if (IsFull())
	{
		return mInvalidOffset;
	}

	if (mTail >= mHead)
	{
		if (mTail + size <= mMaxSize)
		{
			auto offset = mTail;
			mTail += size;
			mUsedSize += size;
			return offset;
		}
		else if (size <= mHead)
		{
			//Allocate from the beginning
			mUsedSize += (mMaxSize - mTail) + size;
			mTail = size;
			return 0;
		}
	}
	else if (mTail + size <= mHead)
	{
		auto offset = mTail;
		mTail += size;
		mUsedSize += size;
		return offset;
	}

	return mInvalidOffset;
}
