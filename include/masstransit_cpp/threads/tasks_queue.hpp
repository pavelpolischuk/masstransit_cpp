#pragma once

#include <masstransit_cpp/global.hpp>

#include <queue>
#include <thread>
#include <future>
#include <type_traits>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API tasks_queue
		{
		public:
			using task_type = std::function<void()>;

			virtual ~tasks_queue() = default;

			template<class F, class... Args>
			auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
			{
				using return_type = typename std::result_of<F(Args...)>::type;

				auto task = std::make_shared<std::packaged_task<return_type()>>(
					std::bind(std::forward<F>(f), std::forward<Args>(args)...));

				std::future<return_type> res = task->get_future();

				{
					std::unique_lock<std::mutex> lock(queue_mutex_);
					if (stop_) throw std::runtime_error("enqueue on stopped tasks_queue");
					tasks_.emplace([task]() { (*task)(); });
				}

				condition_.notify_one();
				return res;
			}

			bool get_task(task_type & task);

			void stop();

		protected:
			mutable std::mutex queue_mutex_;
			mutable std::condition_variable condition_;

			std::queue<task_type> tasks_;
			bool stop_{ false };
		};
	}
}
