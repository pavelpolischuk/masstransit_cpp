#include "masstransit_cpp/threads/worker.hpp"
#include <fstream>

namespace masstransit_cpp
{
	namespace threads
	{
		worker::worker(std::function<void(std::shared_ptr<worker> const&)> const& on_task_completed)
			: future_(std::async(std::launch::async, [this]()
			{
				while (!stop_)
				{
					task_type task;
					
					{
						std::unique_lock<std::mutex> lock(queue_mutex_);
						condition_.wait(lock, [this] { return stop_ || task_.has_value(); });
					
						if (stop_ && !task_.has_value())
							return;
						
						std::swap(task, *task_);
						task_.reset();
					}

					task();
					on_task_completed_(shared_from_this());
				}
			}))
		{
			on_task_completed_.connect(on_task_completed);
		}

		worker::~worker()
		{
			stop();
			attach();
		}

		void worker::attach() const
		{
			if (future_.valid()) future_.wait();
		}

		void worker::stop()
		{
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				stop_ = true;
			}

			condition_.notify_all();
		}

		bool worker::ready() const
		{
			return !stop_ && !task_.has_value() && future_.valid();
		}
	}
}
