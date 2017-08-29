#pragma once

#include <masstransit_cpp/global.hpp>
#include <thread>
#include <future>
#include <type_traits>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_EXPORT task_repeat
		{
		public:
			template<
				class F, class... Args, 
				class D_Rep, class D_Period,
				class = typename std::enable_if<
					std::is_same<typename std::result_of<F(Args...)>::type, bool>::value>::type>
			task_repeat(std::chrono::duration<D_Rep, D_Period> const& wait_interval, F&& f, Args&&... args)
			{
				auto task = std::bind<bool>(std::forward<F>(f), std::forward<Args>(args)...);
				thread_ = std::thread(
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
					} );
			}

			~task_repeat()
			{
				stop();
				thread_.join();
			}

			void stop()
			{
				{
					std::unique_lock<std::mutex> lock(mutex_);
					stop_ = true;
				}

				condition_.notify_all();
			}

		protected:
			std::thread thread_;
			std::mutex mutex_;
			std::condition_variable condition_;
			bool stop_{ false };
		};
	}
}