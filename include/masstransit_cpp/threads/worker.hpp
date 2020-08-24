#pragma once

#include <masstransit_cpp/global.hpp>

#include <future>
#include <optional>
#include <functional>
#include <boost/signals2.hpp>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API worker : public std::enable_shared_from_this<worker>  // NOLINT(hicpp-special-member-functions)
		{
		public:
			using task_type = std::function<void()>;
		
			explicit worker(std::function<void(std::shared_ptr<worker> const&)> const& on_task_completed);
			~worker();

			template<class F, class... Args>
			auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
			{
				using return_type = typename std::result_of<F(Args...)>::type;

				auto task = std::make_shared<std::packaged_task<return_type()>>(
					std::bind(std::forward<F>(f), std::forward<Args>(args)...));

				std::future<return_type> res = task->get_future();

				{
					std::unique_lock<std::mutex> lock(queue_mutex_);
					if (stop_) throw std::runtime_error("enqueue on stopped worker");
					if (task_) throw std::runtime_error("enqueue on busy worker");
					
					task_ = [task] { (*task)(); };
				}

				condition_.notify_one();
				return res;
			}

			void stop();
			void attach() const;

			bool ready() const;

		private:
			mutable std::mutex queue_mutex_;
			mutable std::condition_variable condition_;

			boost::signals2::signal<void(std::shared_ptr<worker> const&)> on_task_completed_;
			std::optional<task_type> task_;
			std::future<void> future_;
			bool stop_{ false };
		};
	}
}
