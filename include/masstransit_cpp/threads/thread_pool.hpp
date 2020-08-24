#pragma once

#include <masstransit_cpp/threads/queue_worker.hpp>

#include <list>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API thread_pool : public tasks_queue
		{
		public:
			explicit thread_pool(size_t pool_size);

			virtual ~thread_pool();

		private:
			std::list<queue_worker> workers_;
		};
	}
}
