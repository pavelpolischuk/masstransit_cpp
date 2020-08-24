#include "message_consumer_threads.hpp"

namespace masstransit_cpp_tests
{
	void message_consumer_threads::consume(consume_context<message_mock> const& context)
	{
		auto thread_id = std::this_thread::get_id();

		{
			std::unique_lock<std::mutex> lock(mutex_);
			if(values_by_threads_.find(thread_id) == values_by_threads_.end())
				values_by_threads_.emplace(thread_id, context.message.id);
		}

		condition_.notify_one();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	void message_consumer_threads::wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::seconds(3), [this] { return values_by_threads_.size() >= NEED_THREAD_COUNT; });
	}

	size_t message_consumer_threads::involved_threads_count() const
	{
		std::unique_lock<std::mutex> lock(mutex_);
		return values_by_threads_.size();
	}
}
