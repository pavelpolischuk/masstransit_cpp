#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/host_info.hpp>
#include <masstransit_cpp/utils/json.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <optional>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API consume_context_info
	{
		boost::uuids::uuid message_id;
		boost::uuids::uuid conversation_id;
		std::optional<boost::uuids::uuid> correlation_id;
		std::optional<boost::uuids::uuid> initiator_id;
		std::string source_address;
		std::string destination_address;

		std::vector<std::string> message_types{}; // example: "urn:message:MyNamespace.MySubNamespace:MessageClassName",

		nlohmann::json message;

		host_info send_host;
		std::map<std::string, std::string> headers{};

		consume_context_info();
		consume_context_info(boost::uuids::uuid const& message_id, std::string const& type, nlohmann::json message);
		consume_context_info(boost::uuids::uuid const& message_id, consume_context_info const& other, std::string const& type,
		                     nlohmann::json message);

		template<typename MessageT>
		static consume_context_info create(MessageT const& message)
		{
			static boost::uuids::random_generator random;
			return { random(), "urn:message:" + MessageT::message_type(), nlohmann::json(message) };
		}

		template<typename MessageT>
		consume_context_info create_in_context(MessageT const& message) const
		{
			static boost::uuids::random_generator random;
			return { random(), *this, "urn:message:" + MessageT::message_type(), nlohmann::json(message) };
		}
		
		friend bool MASSTRANSIT_CPP_API operator==(consume_context_info const& lhv, consume_context_info const& rhv);
	};

	void MASSTRANSIT_CPP_API to_json(nlohmann::json& j, consume_context_info const& p);
	void MASSTRANSIT_CPP_API from_json(nlohmann::json const& j, consume_context_info & p);
}
