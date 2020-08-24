#pragma once

#include <masstransit_cpp/global.hpp>
#include <future>
#include <type_traits>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API task_repeat
		{
		public:
			template<
				class F, class... Args, 
				class DRep, class DPeriod,
				class = typename std::enable_if<std::is_same<typename std::result_of<F(Args...)>::type, bool>::value>::type>
			task_repeat(std::chrono::duration<DRep, DPeriod> const& wait_interval, F&& f, Args&&... args)
			{
				auto task = std::bind<bool>(std::forward<F>(f), std::forward<Args>(args)...);
				future_ = std::async(std::launch::async,
					[wait_interval, task, this]()
					{
						while (!stop_)
						{
							if (!task())
							{
								std::unique_lock<std::mutex> lock(mutex_);
								condition_.wait_for(lock, wait_interval, [this] { return stop_; });
							}
						}
					});
			}

			~task_repeat();

			void wait() const;
			void stop();

		protected:
			std::mutex mutex_;
			std::condition_variable condition_;
			std::future<void> future_;
			bool stop_{ false };
		};
	}
}
