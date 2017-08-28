#pragma once

#include <masstransit_cpp/utils/worker.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT worker_thread : public tasks_queue
	{
	public:
		worker_thread();

		virtual ~worker_thread();

	protected:
		worker worker_;
	};
}