#include "masstransit_cpp/in_memory/receive_endpoint.hpp"

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		receive_endpoint::receive_endpoint(consumers_map const& consumers_by_type)
			: i_receive_endpoint(consumers_by_type)
		{
		}

		bool receive_endpoint::deliver(consume_context_info const& context) const
		{
			auto consumer = find_consumer(context.message_types);
			if (consumer == nullptr)
				return true;

			auto body = context.message.dump(2);
			try
			{
				BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;

				consumer->consume(context);

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

			return true;
		}
	}
}