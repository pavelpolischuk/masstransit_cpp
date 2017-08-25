#include "masstransit_cpp/utils/thread_pool.hpp"

namespace masstransit_cpp
{
	thread_pool::thread_pool(size_t pool_size)
	{
		for (size_t i = 0; i < pool_size; ++i)
			workers_.emplace_back(*this);
	}

	thread_pool::~thread_pool()
	{
		stop();
	}
}