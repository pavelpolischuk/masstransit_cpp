#include "message_consumer_mock.hpp"

namespace masstransit_cpp_tests
{
	message_consumer_mock::message_consumer_mock() = default;
	message_consumer_mock::~message_consumer_mock() = default;

	void message_consumer_mock::consume(consume_context<message_mock> const& context)
	{
		saved_value = context.message.id;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}
