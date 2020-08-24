#include "masstransit_cpp/threads/tasks_queue.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		bool tasks_queue::get_task(task_type& task)
		{
			std::unique_lock<std::mutex> lock(queue_mutex_);
			condition_.wait(lock, 
				[this] { return stop_ || !tasks_.empty(); });
		
			if (stop_ && tasks_.empty())
				return false;

			std::swap(task, tasks_.front());
			tasks_.pop();
			return true;
		}

		void tasks_queue::stop()
		{
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				stop_ = true;
			}

			condition_.notify_all();
		}
	}
}
