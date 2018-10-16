#include "masstransit_cpp/rabbit_mq/message_publisher.hpp"

#include <SimpleAmqpClient/Channel.h>
#include <boost/log/trivial.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace masstransit_cpp
{
	bool message_publisher::publish(consume_context_info const& context,
		boost::shared_ptr<AmqpClient::Channel> const& channel, std::string const& exchange) const
	{
		nlohmann::json json(context);
		auto body = json.dump(2);

		try
		{
			BOOST_LOG_TRIVIAL(debug) << "bus publish message:\n" << body;

			auto amqp_message = AmqpClient::BasicMessage::Create(body);
			amqp_message->MessageId(to_string(context.message_id));

			if (context.correlation_id)
				amqp_message->CorrelationId(to_string(context.correlation_id.get()));

			amqp_message->ContentType("application/vnd.masstransit+json");

			if(!context.headers.empty())
			{
				AmqpClient::Table table;
				for(auto const& it : context.headers)
					table.insert({ it.first, it.second });
				amqp_message->HeaderTable(table);
			}

			channel->BasicPublish(exchange, "", amqp_message);

			BOOST_LOG_TRIVIAL(debug) << "[DONE]";
			return true;
		}
		catch (std::exception & ex)
		{
			BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: " << ex.what() << "\n\tBody: " << body;
			return false;
		}
		catch (...)
		{
			BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: unknown" << "\n\tBody: " << body;
			return false;
		}
	}
}
