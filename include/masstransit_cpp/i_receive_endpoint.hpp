#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>
#include <masstransit_cpp/message_consumer.hpp>

#include <vector>

namespace masstransit_cpp
{
	class i_error_handler;

	class MASSTRANSIT_CPP_API i_receive_endpoint
	{
	public:
		using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;

		i_receive_endpoint(consumers_map const& consumers_by_type, 
			std::shared_ptr<i_publish_endpoint> const& publish_endpoint, 
			std::shared_ptr<i_error_handler> const& error_handler);

		virtual ~i_receive_endpoint() = default;

	protected:
		const consumers_map consumers_by_type_;
		const std::shared_ptr<i_publish_endpoint> publish_endpoint_;
		const std::shared_ptr<i_error_handler> error_handler_;

		std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
		
		template<class MessageT>
		void deliver_impl(consume_context<MessageT> const& context) const
		{
			auto consumer = std::static_pointer_cast<message_consumer<MessageT>>(find_consumer(context.message_types));
			if (consumer == nullptr)
				return;

			const auto body = nlohmann::json(context.message).dump(2);
			try
			{
				consumer->consume(context);
			}
			catch (std::exception & ex)
			{
				on_error(context.info, consumer->message_type(), body, ex);
			}
			catch (...)
			{
				on_error(context.info, consumer->message_type(), body, std::runtime_error("unknown"));
			}
		}
		
		void deliver_impl(consume_context_info const& context) const;

		virtual void on_error(consume_context_info const& context, std::string const& consumer_type, std::exception const& ex) const;
	};
}
