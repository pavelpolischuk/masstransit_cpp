#include "message_consumer_depend.hpp"

#include <masstransit_cpp/i_publish_endpoint.hpp>

namespace masstransit_cpp_tests
{
	void message_consumer_depend::consume(consume_context<message_mock> const& context)
	{
		if (context.message.id == PUBLISH_WITH_CONTEXT)
		{
			context.publish(message_mock(PUBLISH_WITH_CONTEXT + 1));
			return;
		}
		
		{
			std::unique_lock<std::mutex> lock(mutex_);
			saved_value = context.message.id;
		}

		condition_.notify_all();
	}

	void message_consumer_depend::wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::seconds(2), [this] { return saved_value.has_value(); });
	}
}
