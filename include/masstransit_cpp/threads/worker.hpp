#pragma once
#include <masstransit_cpp/threads/tasks_queue.hpp>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_EXPORT worker
		{
		public:
			explicit worker(tasks_queue & queue);
			~worker();

		protected:
			std::thread thread_;
		};
	}
}