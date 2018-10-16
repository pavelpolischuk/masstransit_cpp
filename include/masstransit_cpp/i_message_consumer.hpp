#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API i_message_consumer
	{
	public:
		virtual ~i_message_consumer() = default;

		virtual void consume(consume_context_info const& context, std::shared_ptr<i_publish_endpoint> const& publish_endpoint) = 0;
		
		virtual std::string message_type() const = 0;
	};
}
