#include "message_consumer_threads.hpp"

namespace masstransit_cpp_tests
{
	const int message_consumer_threads::NEED_THREAD_COUNT = 3;

	void message_consumer_threads::consume(consume_context<message_mock> const& context)
	{
		auto thread_id = std::this_thread::get_id();

		{
			std::unique_lock<std::mutex> lock(mutex_);
			values_by_threads_[thread_id] = context.message.id;
		}

		if (values_by_threads_.size() >= NEED_THREAD_COUNT) 
			condition_.notify_all();
		else 
			wait();
	}

	void message_consumer_threads::wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::seconds(6), [this] { return values_by_threads_.size() >= NEED_THREAD_COUNT; });
	}

	size_t message_consumer_threads::involved_threads_count() const
	{
		std::unique_lock<std::mutex> lock(mutex_);
		return values_by_threads_.size();
	}
}
