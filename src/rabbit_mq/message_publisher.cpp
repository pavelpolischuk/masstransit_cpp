#include "masstransit_cpp/rabbit_mq/message_publisher.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"
#include "masstransit_cpp/utils/i_error_handler.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace masstransit_cpp
{
	message_publisher::message_publisher(std::shared_ptr<i_error_handler> error_handler)
		: error_handler_(std::move(error_handler))
	{
	}

	bool message_publisher::publish(consume_context_info const& context,
		std::shared_ptr<rabbit_mq::amqp_channel> const& channel, std::string const& exchange) const
	{
		nlohmann::json json(context);
		const auto body = json.dump(2);

		try
		{
			auto message = std::make_shared<rabbit_mq::amqp_message>(body);
			message->set_message_id(to_string(context.message_id));

			if (context.correlation_id)
				message->set_correlation_id(to_string(context.correlation_id.value()));

			message->set_content_type("application/vnd.masstransit+json");

			if(!context.headers.empty())
			{
				rabbit_mq::amqp_table table;
				for(auto const& it : context.headers)
					table.insert({ it.first, it.second });
				message->set_header_table(table);
			}

			channel->publish(exchange, "", message);
			return true;
		}
		catch (std::exception & ex)
		{
			error_handler_->on_error("message_publisher::publish", ex.what());
			return false;
		}
		catch (...)
		{
			error_handler_->on_error("message_publisher::publish", "");
			return false;
		}
	}
}
