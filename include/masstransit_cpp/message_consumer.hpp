#pragma once
#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/consume_context.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT i_message_consumer
	{
	public:
		virtual ~i_message_consumer() = default;

		virtual void consume(consume_context_info const& context) = 0;
		
		virtual std::string message_name() const = 0;
	};

	template<typename message_t, typename std::enable_if<
		std::is_convertible<nlohmann::json, message_t>::value, int>::type = 0 >
	class MASSTRANSIT_CPP_EXPORT message_consumer : public i_message_consumer
	{
	public:
		using message_type = message_t;

		message_consumer() {}
		virtual ~message_consumer() = default;

		std::string message_name() const override 
		{ 
			return message_t::message_type();
		}

		void consume(consume_context_info const& context) override
		{
			consume(consume_context<message_t>(context));
		}

		virtual void consume(consume_context<message_t> const& context) = 0;
	};
}
