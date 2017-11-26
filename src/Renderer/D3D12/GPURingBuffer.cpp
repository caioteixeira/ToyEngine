#include "GPURingBuffer.h"


GPURingBuffer::GPURingBuffer(size_t maxSize, ID3D12Device* device, bool allowCPUAccess)
	: mMaxSize(maxSize), mCPUVirtualAddress(nullptr), mGPUVirtualAddress(0)
{
	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES defaultUsage;
	if(allowCPUAccess)
	{
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		defaultUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	else
	{
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		defaultUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	desc.Width = maxSize;

	auto hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, 
		defaultUsage, nullptr, __uuidof(mBuffer), &mBuffer);
	ThrowIfFailed(hr, "Failed to create ring buffer!");
	mBuffer->SetName(L"Upload Ring Buffer");

	mGPUVirtualAddress = mBuffer->GetGPUVirtualAddress();

	if(allowCPUAccess)
	{
		mBuffer->Map(0, nullptr, &mCPUVirtualAddress);
	}
}

GPURingBuffer::~GPURingBuffer()
{
	Destroy();
}

GPURingBuffer::GPURingBuffer(GPURingBuffer && rhs) noexcept :
	mCPUVirtualAddress(rhs.mCPUVirtualAddress),
	mGPUVirtualAddress(rhs.mGPUVirtualAddress),
	mMaxSize(rhs.mMaxSize),
	mUsedSize(rhs.mUsedSize),
	mTail(rhs.mTail),
	mHead(rhs.mHead),
	mBuffer(std::move(rhs.mBuffer))
{
	rhs.mCPUVirtualAddress = nullptr;
	rhs.mGPUVirtualAddress = 0;
	rhs.mMaxSize = 0;
	rhs.mUsedSize = 0;
	rhs.mHead = 0;
	rhs.mTail = 0;
	rhs.mBuffer = nullptr;
}

GPURingBuffer & GPURingBuffer::operator=(GPURingBuffer && rhs) noexcept
{
	Destroy();
	mCPUVirtualAddress = rhs.mCPUVirtualAddress;
	mGPUVirtualAddress = rhs.mGPUVirtualAddress;
	mMaxSize = rhs.mMaxSize;
	mUsedSize = rhs.mUsedSize;
	mTail = rhs.mTail;
	mHead = rhs.mHead;
	mBuffer = std::move(rhs.mBuffer);
	rhs.mCPUVirtualAddress = 0;
	rhs.mGPUVirtualAddress = 0;
	rhs.mMaxSize = 0;
	rhs.mUsedSize = 0;
	rhs.mTail = 0;
	rhs.mHead = 0;

	return *this;
}

DynamicAllocation GPURingBuffer::Allocate(size_t size)
{
	auto offset = GetAllocationOffset(size);

	if(offset != invalidOffset)
	{
		DynamicAllocation allocation(mBuffer.Get(), offset, size);
		allocation.GPUAddress = mGPUVirtualAddress + offset;
		allocation.CPUAddress = mCPUVirtualAddress;
		if(allocation.CPUAddress)
		{
			allocation.CPUAddress = reinterpret_cast<char *>(allocation.CPUAddress) + offset;
		}
		return allocation;
	}
	else
	{
		//TODO: Log error
		return DynamicAllocation(nullptr, 0, 0);
	}
}

void GPURingBuffer::FinishCurrentFrame(uint64_t frameNum)
{
	mCompletedFrameTails.push_back(std::make_pair(frameNum, mTail));
}

void GPURingBuffer::ReleaseCompletedFrames(uint64_t numCompletedFrames)
{
	while(!mCompletedFrameTails.empty() && mCompletedFrameTails.front().first < numCompletedFrames)
	{
		auto &oldestFrameTail = mCompletedFrameTails.front().second;
		const auto completed = mCompletedFrameTails.front().first;
		if(mUsedSize > 0)
		{
			if(oldestFrameTail > mHead)
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

size_t GPURingBuffer::GetAllocationOffset(size_t size)
{
	if(IsFull())
	{
		return invalidOffset;
	}

	if(mTail >= mHead)
	{
		if(mTail + size <= mMaxSize)
		{
			auto offset = mTail;
			mTail += size;
			mUsedSize += size;
			return offset;
		}
		else if(size <= mHead)
		{
			//Allocate from the beginning
			mUsedSize += (mMaxSize - mTail) + size;
			mTail = size;
			return 0;
		}
	}
	else if(mTail + size <= mHead)
	{
		auto offset = mTail;
		mTail += size;
		mUsedSize += size;
		return offset;
	}

	return invalidOffset;
}

void GPURingBuffer::Destroy()
{
	if (mCPUVirtualAddress)
	{
		mBuffer->Unmap(0, nullptr);
	}
	
	if(mBuffer != nullptr)
	{
		mBuffer.Reset();
	}

	mCPUVirtualAddress = 0;
	mGPUVirtualAddress = 0;
}