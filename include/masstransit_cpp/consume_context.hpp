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

		template<typename message_t>
		static consume_context_info create(message_t const& message)
		{
			consume_context_info info;
			info.message = nlohmann::json(message);
			info.message_types = { "urn:message:" + message_t::message_type() };
			boost::uuids::random_generator random;
			info.message_id = random();
			info.conversation_id = random();
			return std::move(info);
		}

		boost::uuids::uuid message_id;
		boost::uuids::uuid conversation_id;
		std::string source_address;
		std::string destination_address;
		
		std::vector<std::string> message_types; // example: "urn:message:MyNamespace.MySubNamespace:MessageClassName",

		nlohmann::json message;

		host_info send_host;

		friend bool operator==(consume_context_info const& lhv, consume_context_info const& rhv);
	};

	void to_json(nlohmann::json& j, consume_context_info const& p);
	void from_json(nlohmann::json const& j, consume_context_info & p);


	template<class message_t>
	struct MASSTRANSIT_CPP_EXPORT consume_context
	{
		using message_type = message_t;

		explicit consume_context(consume_context_info const& info)
			: info(info)
			, message(info.message.get<message_type>())
		{}

		consume_context_info info;
		message_type message;
	};
}