#include "WorkerPool.h"
#include <easy/profiler.h>

Engine::Core::WorkerPool::WorkerPool(int threads)
{
	for(int i = 0; i < threads; ++i)
	{
		mThreads.emplace_back([this]()
		{
			EASY_THREAD("Worker Thread")
			mQueue.work_forever();
		});
	}
}

Engine::Core::WorkerPool::~WorkerPool()
{
	mQueue.close();
	for(auto& thread: mThreads)
	{
		thread.join();
	}
}
