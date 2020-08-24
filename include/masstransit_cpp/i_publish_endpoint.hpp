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
			return publish(consume_context_info::create(message), MessageT::message_type());
		}

		virtual std::future<bool> publish(consume_context_info message, std::string const& exchange) const = 0;
	};
}
