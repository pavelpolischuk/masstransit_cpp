#include "masstransit_cpp/threads/queue_worker.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		queue_worker::queue_worker(tasks_queue & queue)
			: future_(std::async(std::launch::async, [&queue]()
			{
				tasks_queue::task_type task;
				while (queue.get_task(task))
				{
					task();
				}
			}))
		{}

		queue_worker::~queue_worker()
		{
			attach();
		}

		void queue_worker::attach() const
		{
			if (future_.valid()) future_.wait();
		}
	}
}
