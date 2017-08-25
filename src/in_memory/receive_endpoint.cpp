#include "masstransit_cpp/in_memory/receive_endpoint.hpp"

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		receive_endpoint::receive_endpoint(std::map<std::string, std::shared_ptr<i_message_consumer>> const& consumers_by_type)
			: consumers_by_type_(consumers_by_type)
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

		std::shared_ptr<i_message_consumer> receive_endpoint::find_consumer(std::vector<std::string> const& message_types) const
		{
			for (auto const& message_type : message_types)
			{
				auto consumer = consumers_by_type_.find(message_type);
				if (consumer != consumers_by_type_.end())
					return consumer->second;
			}

			return nullptr;
		}
	}
}