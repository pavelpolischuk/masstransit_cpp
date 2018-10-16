#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context_info.hpp>

#include <future>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API i_publish_endpoint
	{
	public:
		virtual ~i_publish_endpoint() = default;

		template<typename MessageT>
		std::future<bool> publish(MessageT const& message) const
		{
			consume_context_info info = consume_context_info::create(message);
			fill(info, MessageT::message_type());
			return publish(info, MessageT::message_type());
		}

		virtual std::future<bool> publish(consume_context_info const& message, std::string const& exchange) const = 0;

	protected:
		virtual void fill(consume_context_info & message, std::string const& exchange) const {}
	};
}
