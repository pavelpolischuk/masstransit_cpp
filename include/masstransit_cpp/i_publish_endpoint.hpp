#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context.hpp>
#include <masstransit_cpp/json.hpp>

#include <future>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT i_publish_endpoint
	{
	public:
		virtual ~i_publish_endpoint() = default;

		template<typename message_t, typename std::enable_if<
			std::is_convertible<nlohmann::json, message_t>::value, int>::type = 0 >
		std::future<bool> publish(message_t const& message) const
		{
			consume_context_info info = consume_context_info::create(message);
			return publish_impl(info, message_t::message_type());
		}

	protected:
		virtual std::future<bool> publish_impl(consume_context_info const& message, std::string const& type) const = 0;
	};
}
