#pragma once
/*
* Strongly based on Diligent Engine's Variable size allocator:
*  https://www.codeproject.com/Articles/1180070/Simple-Variable-Size-Memory-Block-Allocator
*/

#include <map>


class VariableSizeMemoryBlockAllocator
{
	static const size_t INVALID_OFFSET = static_cast<size_t>(-1);

	struct FreeBlockInfo;
	using FreeBlocksByOffsetMap = std::map<size_t, FreeBlockInfo>;
	using FreeBlocksBySizeMap = std::multimap<size_t, FreeBlocksByOffsetMap::iterator>;

	struct FreeBlockInfo
	{
		size_t size;

		FreeBlocksBySizeMap::iterator orderBySizeIterator;

		FreeBlockInfo(size_t size) : size(size) {};
	};


public:
	VariableSizeMemoryBlockAllocator();
	~VariableSizeMemoryBlockAllocator();

	size_t Allocate(size_t size);
	void Free(size_t offset, size_t size);

	size_t GetMaxSize() const { return mMaxSize; }
	bool IsFull() const { return mFreeSize == 0; };
	bool IsEmpty() const { return mFreeSize == mMaxSize; };
	size_t GetFreeSize() const { return mFreeSize; }

private:
	inline void AddNewBlock(size_t offset, size_t size);

	FreeBlocksByOffsetMap mFreeBlocksByOffset;
	FreeBlocksBySizeMap mFreeBlocksBySize;

	size_t mMaxSize = 0;
	size_t mFreeSize = 0;

#ifdef _DEBUG
	void DebugVerifyList();
#endif
};

