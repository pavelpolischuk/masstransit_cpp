#pragma once

#include <masstransit_cpp/threads/tasks_queue.hpp>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API queue_worker
		{
		public:
			explicit queue_worker(tasks_queue & queue);
			~queue_worker();

			void attach() const;

		protected:
			std::future<void> future_;
		};
	}
}
