#include "message_consumer_depend.hpp"

#include <masstransit_cpp/i_publish_endpoint.hpp>

namespace masstransit_cpp_tests
{
	bus_dependent_message_consumer::bus_dependent_message_consumer(std::shared_ptr<i_publish_endpoint> const& bus)
		: bus_(bus)
	{}

	void bus_dependent_message_consumer::consume(consume_context<message_mock> const& context)
	{
		switch (context.message.id)
		{
		case PUBLISH_WITH_BUS_DEPS:
			bus_->publish(message_mock(PUBLISH_WITH_BUS_DEPS+1));
			return;
		case PUBLISH_WITH_CONTEXT:
			context.publish(message_mock(PUBLISH_WITH_CONTEXT + 1));
			return;
		default:
		{
			std::unique_lock<std::mutex> lock(mutex_);
			saved_value = context.message.id;
		}

			condition_.notify_all();
		}
	}

	void bus_dependent_message_consumer::wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::seconds(2), [this] { return saved_value != boost::none; });
	}
}
