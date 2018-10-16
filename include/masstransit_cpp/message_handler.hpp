#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_consumer.hpp>

namespace masstransit_cpp
{
	template<typename message_t, typename std::enable_if<
		std::is_convertible<nlohmann::json, message_t>::value, int>::type = 0 >
	class MASSTRANSIT_CPP_API message_handler : public message_consumer<message_t>
	{
	public:
		using type_message = message_t;

		message_handler(std::function<void(consume_context<type_message> const&)> const& handler)
			: handler_(handler)
		{}

		virtual ~message_handler() = default;

		void consume(consume_context<type_message> const& context) override
		{
			handler_(context);
		}

	private:
		std::function<void(consume_context<type_message> const&)> handler_;
	};
}
