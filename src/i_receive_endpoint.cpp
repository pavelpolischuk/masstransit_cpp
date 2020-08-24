#include "masstransit_cpp/i_receive_endpoint.hpp"
#include "masstransit_cpp/utils/i_error_handler.hpp"

namespace masstransit_cpp
{
	i_receive_endpoint::i_receive_endpoint(consumers_map const& consumers_by_type, 
		std::shared_ptr<i_publish_endpoint> const& publish_endpoint, 
		std::shared_ptr<i_error_handler> const& error_handler)
		: consumers_by_type_(consumers_by_type)
		, publish_endpoint_(publish_endpoint)
		, error_handler_(error_handler)
	{
	}

	std::shared_ptr<i_message_consumer> i_receive_endpoint::find_consumer(std::vector<std::string> const& message_types) const
	{
		for (auto const& message_type : message_types)
		{
			const auto consumer = consumers_by_type_.find(message_type);
			if (consumer != consumers_by_type_.end())
				return consumer->second;
		}

		return nullptr;
	}

	void i_receive_endpoint::deliver_impl(consume_context_info const& context) const
	{
		auto consumer = find_consumer(context.message_types);
		if (consumer == nullptr)
			return;

		const auto body = context.message.dump(2);
		try
		{
			consumer->consume(context, publish_endpoint_);
		}
		catch (std::exception & ex)
		{
			on_error(context, consumer->message_type(), ex);
		}
		catch (...)
		{
			on_error(context, consumer->message_type(), std::runtime_error("unknown"));
		}
	}

	void i_receive_endpoint::on_error(consume_context_info const& context, std::string const& consumer_type, std::exception const& ex) const
	{
		error_handler_->on_error("i_receive_endpoint[" + consumer_type + "]::deliver_impl", ex.what());
	}
}
