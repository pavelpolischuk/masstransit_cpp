#pragma once

#include <masstransit_cpp/threads/worker.hpp>

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
			std::list<worker> workers_;
		};
	}
}