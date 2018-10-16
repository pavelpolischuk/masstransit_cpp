#include "message_mock.hpp"

#include <masstransit_cpp/message_consumer.hpp>
#include <boost/optional.hpp>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::message_consumer;
	using ::masstransit_cpp::consume_context;

	class message_consumer_mock : public message_consumer<message_mock>
	{
	public:
		boost::optional<int> saved_value;

		message_consumer_mock(){}
		~message_consumer_mock() override = default;
		
		void consume(consume_context<message_mock> const& context) override
		{
			saved_value = context.message.id;
		}
	};
}
