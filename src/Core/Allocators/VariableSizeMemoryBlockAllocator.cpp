#include "VariableSizeMemoryBlockAllocator.h"
#include <cassert>


VariableSizeMemoryBlockAllocator::VariableSizeMemoryBlockAllocator()
{
}


VariableSizeMemoryBlockAllocator::~VariableSizeMemoryBlockAllocator()
{
}

#ifdef _DEBUG
void VariableSizeMemoryBlockAllocator::DebugVerifyList()
{
	size_t TotalFreeSize = 0;

	auto BlockIt = mFreeBlocksByOffset.begin();
	auto PrevBlockIt = mFreeBlocksByOffset.end();
	assert(mFreeBlocksByOffset.size() == mFreeBlocksBySize.size());
	while (BlockIt != mFreeBlocksByOffset.end())
	{
		assert(BlockIt->first >= 0 && BlockIt->first + BlockIt->second.size <= mMaxSize);
		assert(BlockIt == BlockIt->second.orderBySizeIterator->second);
		assert(BlockIt->second.size == BlockIt->second.orderBySizeIterator->first);
		//   PrevBlock.Offset                   BlockIt.first                     
		//     |                                  |                            
		// ~ ~ |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->| ~ ~ ~
		//
		// Assert: "Unmerged adjacent or overlapping blocks detected"
		assert(PrevBlockIt == mFreeBlocksByOffset.end() || BlockIt->first > PrevBlockIt->first + PrevBlockIt->second.size);
		TotalFreeSize += BlockIt->second.size;

		PrevBlockIt = BlockIt;
		++BlockIt;
	}

	auto OrderIt = mFreeBlocksBySize.begin();
	while (OrderIt != mFreeBlocksBySize.end())
	{
		assert(OrderIt->first == OrderIt->second->second.size);
		++OrderIt;
	}

	assert(TotalFreeSize == mFreeSize);
}
#endif

size_t VariableSizeMemoryBlockAllocator::Allocate(size_t size)
{
	assert(size > 0);

	if (mFreeSize < size)
	{
		return INVALID_OFFSET;
	}

	auto smallestBlockItIt = mFreeBlocksBySize.lower_bound(size);
	if (smallestBlockItIt == mFreeBlocksBySize.end())
	{
		return INVALID_OFFSET;
	}

	auto smallestBlockIt = smallestBlockItIt->second;
	assert(size <= smallestBlockIt->second.size);
	assert(smallestBlockIt->second.size == smallestBlockIt->first);

	auto offset = smallestBlockIt->first;
	auto newOffset = offset - size;
	auto newSize = smallestBlockIt->second.size - size;

	assert(smallestBlockItIt == smallestBlockIt->second.orderBySizeIterator);

	mFreeBlocksBySize.erase(smallestBlockItIt);
	mFreeBlocksByOffset.erase(smallestBlockIt);

	if (newSize > 0)
	{
		AddNewBlock(newOffset, newSize);
	}

	mFreeSize -= size;

#ifdef _DEBUG
	DebugVerifyList();
#endif

	return offset;
}

void VariableSizeMemoryBlockAllocator::Free(size_t offset, size_t size)
{
	assert(offset + size <= mMaxSize);

	auto nextBlockIt = mFreeBlocksByOffset.upper_bound(offset);

#ifdef _DEBUG
	{
		auto LowBnd = mFreeBlocksByOffset.lower_bound(offset);
		assert (LowBnd == nextBlockIt);
	}
#endif

	// Block being deallocated must not overlap with the next block
	assert(nextBlockIt == mFreeBlocksByOffset.end() || offset + size <= nextBlockIt->first);
	auto prevBlockIt = nextBlockIt;
	if (prevBlockIt != mFreeBlocksByOffset.begin())
	{
		--prevBlockIt;
		assert(offset >= prevBlockIt->first + prevBlockIt->second.size);
	}
	else
	{
		prevBlockIt = mFreeBlocksByOffset.end();
	}

	size_t newSize;
	size_t newOffset;
	if(prevBlockIt != mFreeBlocksByOffset.end() && offset == prevBlockIt->first + prevBlockIt->second.size)
	{
		newSize = prevBlockIt->second.size + size;
		newOffset = prevBlockIt->first;

		if (nextBlockIt != mFreeBlocksByOffset.end() && offset + size == nextBlockIt->first)
		{
			newSize += nextBlockIt->second.size;
			mFreeBlocksBySize.erase(prevBlockIt->second.orderBySizeIterator);
			mFreeBlocksBySize.erase(nextBlockIt->second.orderBySizeIterator);
			//Delete the range of two blocks
			++nextBlockIt;
			mFreeBlocksByOffset.erase(prevBlockIt, nextBlockIt);
		}
		else
		{
			mFreeBlocksBySize.erase(prevBlockIt->second.orderBySizeIterator);
			mFreeBlocksByOffset.erase(prevBlockIt);
		}
	}
	else if (nextBlockIt != mFreeBlocksByOffset.end() && offset + size == nextBlockIt->first)
	{
		newSize = size + nextBlockIt->second.size;
		newOffset = offset;
		mFreeBlocksBySize.erase(nextBlockIt->second.orderBySizeIterator);
		mFreeBlocksByOffset.erase(nextBlockIt);
	}
	else
	{
		newSize = size;
		newOffset = offset;
	}

	AddNewBlock(newOffset, newSize);
	mFreeSize += size;

#ifdef _DEBUG
	DebugVerifyList();
#endif
}

inline void VariableSizeMemoryBlockAllocator::AddNewBlock(size_t offset, size_t size)
{
	auto newBlockIt = mFreeBlocksByOffset.emplace(offset, size);
	auto orderIt = mFreeBlocksBySize.emplace(size, newBlockIt.first);
	newBlockIt.first->second.orderBySizeIterator = orderIt;
}
