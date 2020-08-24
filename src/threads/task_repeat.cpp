#include "masstransit_cpp/threads/task_repeat.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		task_repeat::~task_repeat()
		{
			stop();
			wait();
		}

		void task_repeat::wait() const
		{
			if (future_.valid()) future_.wait();
		}

		void task_repeat::stop()
		{
			{
				std::unique_lock<std::mutex> lock(mutex_);
				stop_ = true;
			}

			condition_.notify_all();
		}
	}
}
