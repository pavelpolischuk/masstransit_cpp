#include "message_consumer_depend.hpp"

#include <masstransit_cpp/i_publish_endpoint.hpp>

namespace masstransit_cpp_tests
{
	message_consumer_depend::message_consumer_depend(std::shared_ptr<i_publish_endpoint> const& bus)
		: bus_(bus)
	{}

	void message_consumer_depend::consume(consume_context<message_mock> const& context)
	{
		if (context.message.id == 42)
		{
			bus_->publish(message_mock(43));
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
		condition_.wait_for(lock, std::chrono::seconds(2), [this] { return saved_value != boost::none; });
	}
}
