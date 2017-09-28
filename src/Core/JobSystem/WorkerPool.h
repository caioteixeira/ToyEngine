#pragma once
#include <vector>
#include <thread>
#include "queue.h"

namespace Engine::Core
{
	class WorkerPool
	{
	public:
		explicit WorkerPool(int threads);
		~WorkerPool();

		jobxx::queue& GetQueue() { return mQueue; }
	private:
		jobxx::queue mQueue;
		std::vector<std::thread> mThreads;
	};
}
