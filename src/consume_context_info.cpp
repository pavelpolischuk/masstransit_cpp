#include "masstransit_cpp/consume_context_info.hpp"
#include "masstransit_cpp/utils/json_adapters.hpp"
#include <utility>

namespace masstransit_cpp
{
	consume_context_info::consume_context_info()
		: message_id()
		, conversation_id()
	{
	}

	consume_context_info::consume_context_info(boost::uuids::uuid const& message_id, std::string const& type, nlohmann::json message)
		: message_id(message_id)
		, conversation_id(message_id)
		, message_types({ type })
		, message(std::move(message))
	{
	}

	consume_context_info::consume_context_info(boost::uuids::uuid const& message_id, consume_context_info const& other, std::string const& type, nlohmann::json message)
		: message_id(message_id)
		, conversation_id(other.conversation_id)
		, correlation_id(other.correlation_id)
		, message_types({ type })
		, message(std::move(message))
	{
		initiator_id = correlation_id ? correlation_id : other.message_id;
	}

	void to_json(nlohmann::json& j, consume_context_info const& p)
	{
		j = {
			{ "messageId", p.message_id },
			{ "conversationId", p.conversation_id },
			{ "sourceAddress", p.source_address },
			{ "destinationAddress", p.destination_address },
			{ "messageType", p.message_types },
			{ "message", p.message },
			{ "host", p.send_host }
		};

		if(p.correlation_id)
		{
			j["correlationId"] = p.correlation_id.value();
		}
		if(p.initiator_id)
		{
			j["initiatorId"] = p.initiator_id.value();
		}
	}

	void from_json(nlohmann::json const& j, consume_context_info& p)
	{
		p.message_id = j.at("messageId").get<boost::uuids::uuid>();
		p.conversation_id = j.at("conversationId").get<boost::uuids::uuid>();
		p.source_address = j.at("sourceAddress").get<std::string>();
		p.destination_address = j.at("destinationAddress").get<std::string>();
		p.message_types = j.at("messageType").get<std::vector<std::string>>();
		p.message = j.at("message").get<nlohmann::json::object_t>();
		p.send_host = j.at("host").get<host_info>();

		const auto correlation_id = j.find("correlationId");
		if(correlation_id != j.end())
		{
			p.correlation_id = correlation_id->get<boost::uuids::uuid>();
		}
		const auto initiator_id = j.find("initiatorId");
		if(initiator_id != j.end())
		{
			p.initiator_id = initiator_id->get<boost::uuids::uuid>();
		}
	}

	bool operator==(consume_context_info const& lhv, consume_context_info const& rhv)
	{
		return
			lhv.message_id == rhv.message_id &&
			lhv.conversation_id == rhv.conversation_id &&
			lhv.correlation_id == rhv.correlation_id &&
			lhv.initiator_id == rhv.initiator_id &&
			lhv.source_address == rhv.source_address &&
			lhv.destination_address == rhv.destination_address &&
			lhv.message_types == rhv.message_types &&
			lhv.message == rhv.message &&
			lhv.send_host == rhv.send_host;
	}
}
