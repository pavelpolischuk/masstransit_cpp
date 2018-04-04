#include "masstransit_cpp/threads/worker.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		worker::worker(tasks_queue & queue)
			: future_(std::async(std::launch::async, [this, &queue]()
				{
					while (true)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(queue.queue_mutex_);
							queue.condition_.wait(lock, [this, &queue]
							{
								return queue.stop_ || !queue.tasks_.empty(); 
							});
						
							if (queue.stop_ && queue.tasks_.empty())
								return;

							task = std::move(queue.tasks_.front());
							queue.tasks_.pop();
						}

						task();
					}
				}))
		{
		}

		worker::~worker()
		{
			if (future_.valid()) future_.wait();
		}

		void worker::attach() const
		{
			if (future_.valid()) future_.wait();
		}
	}
}
