#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>

namespace masstransit_cpp
{
	template<class MessageT>
	struct MASSTRANSIT_CPP_API consume_context
	{
		using message_type = MessageT;

		consume_context_info info;
		message_type message;

		consume_context() = delete;

		consume_context(consume_context_info const& info, std::shared_ptr<i_publish_endpoint> const& publish_endpoint)
			: info(info)
			, message(info.message.get<message_type>())
			, publish_endpoint_(publish_endpoint)
		{}

		template<typename OtherMessageT>
		std::future<bool> publish(OtherMessageT const& message) const
		{
			return publish_endpoint_->publish(message);
		}

	private:
		std::shared_ptr<i_publish_endpoint> publish_endpoint_;
	};
}
