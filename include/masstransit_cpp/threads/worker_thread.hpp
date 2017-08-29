#pragma once

#include <masstransit_cpp/threads/worker.hpp>

namespace masstransit_cpp
{
	namespace threads
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
}