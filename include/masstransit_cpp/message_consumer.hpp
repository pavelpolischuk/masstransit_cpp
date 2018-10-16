#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_message_consumer.hpp>

namespace masstransit_cpp
{
	template<class MessageT>
	class MASSTRANSIT_CPP_API message_consumer : public i_message_consumer
	{
	public:
		using type_message = MessageT;

		message_consumer() {}
		virtual ~message_consumer() = default;

		std::string message_type() const override
		{ 
			return type_message::message_type();
		}

		void consume(consume_context_info const& context, std::shared_ptr<i_publish_endpoint> const& publish_endpoint) override
		{
			consume(consume_context<type_message>(context, publish_endpoint));
		}

		virtual void consume(consume_context<type_message> const& context) = 0;
	};
}
