#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context_info.hpp>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API message_publisher
	{
	public:
		bool publish(consume_context_info const& context, boost::shared_ptr<AmqpClient::Channel> const& channel, std::string const& exchange) const;
	};
}
