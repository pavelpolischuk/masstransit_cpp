#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context_info.hpp>

namespace masstransit_cpp
{
	class i_error_handler;
	namespace rabbit_mq { class amqp_channel; }

	class MASSTRANSIT_CPP_API message_publisher
	{
	public:
		explicit message_publisher(std::shared_ptr<i_error_handler> error_handler);

		bool publish(consume_context_info const& context, std::shared_ptr<rabbit_mq::amqp_channel> const& channel, std::string const& exchange) const;

	private:
		const std::shared_ptr<i_error_handler> error_handler_;
	};
}
