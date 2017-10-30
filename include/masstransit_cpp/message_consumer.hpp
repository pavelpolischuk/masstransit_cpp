#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_message_consumer.hpp>

namespace masstransit_cpp
{
	template<class message_t>
	class MASSTRANSIT_CPP_EXPORT message_consumer : public i_message_consumer
	{
	public:
		using type_message = message_t;

		message_consumer() {}
		virtual ~message_consumer() = default;

		std::string message_type() const override
		{ 
			return type_message::message_type();
		}

		void consume(consume_context_info const& context) override
		{
			consume(consume_context<type_message>(context));
		}

		virtual void consume(consume_context<type_message> const& context) = 0;
	};
}