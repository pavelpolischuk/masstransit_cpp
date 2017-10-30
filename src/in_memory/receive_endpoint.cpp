#include "masstransit_cpp/in_memory/receive_endpoint.hpp"

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		receive_endpoint::receive_endpoint(std::string const& queue, consumers_map const& consumers_by_type)
			: i_receive_endpoint(consumers_by_type)
			, queue_(queue)
		{
		}

		void receive_endpoint::deliver(consume_context_info const& context)
		{
			consumer_worker_.enqueue([this](consume_context_info const& message) {
				auto message_context = message;
				auto consumer = find_consumer(message.message_types);
				if (consumer == nullptr)
					return;

				auto body = message.message.dump(2);
				try
				{
					BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;

					consumer->consume(message);

					BOOST_LOG_TRIVIAL(debug) << "[DONE]";
				}
				catch (std::exception & ex)
				{
					BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
						<< body << "\n\tException: " << ex.what();
				}
				catch (...)
				{
					BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
						<< body << "\n\tException: unknown";
				}
			}, context);
		}
	}
}