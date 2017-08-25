#include "masstransit_cpp/utils/worker_thread.hpp"

namespace masstransit_cpp
{
	worker_thread::worker_thread()
		: worker_(*this)
	{
	}

	worker_thread::~worker_thread()
	{
		stop();
	}
}