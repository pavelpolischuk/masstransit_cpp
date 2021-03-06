#pragma once

#include <masstransit_cpp/threads/queue_worker.hpp>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API worker_thread : public tasks_queue
		{
		public:
			worker_thread();
			virtual ~worker_thread();

			void wait() const;

		protected:
			queue_worker worker_;
		};
	}
}
