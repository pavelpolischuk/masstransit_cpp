#pragma once
#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/json.hpp>
#include <masstransit_cpp/host_info.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_EXPORT consume_context_info
	{
		consume_context_info();
		explicit consume_context_info(nlohmann::json const& json);

		template<typename message_t>
		static consume_context_info create(message_t const& message)
		{
			consume_context_info info;
			info.message = message.to_json();
			info.message_types = { "urn:message:" + message_t::message_type() };
			boost::uuids::random_generator random;
			info.message_id = random();
			info.conversation_id = random();
			return std::move(info);
		}

		nlohmann::json to_json() const;

		boost::uuids::uuid message_id;
		boost::uuids::uuid conversation_id;
		std::string source_address;
		std::string destination_address;
		
		std::vector<std::string> message_types; // example: "urn:message:MyNamespace.MySubNamespace:MessageClassName",

		nlohmann::json message;

		host_info send_host;

		friend bool operator==(consume_context_info const& lhv, consume_context_info const& rhv);
	};


	template<typename message_t, typename std::enable_if<
		std::is_convertible<nlohmann::json, message_t>::value, int>::type = 0>
	struct MASSTRANSIT_CPP_EXPORT consume_context
	{
		using message_type = message_t;

		explicit consume_context(consume_context_info const& info)
			: info(info)
			, message(info.message)
		{}

		consume_context_info info;
		message_type message;
	};
}