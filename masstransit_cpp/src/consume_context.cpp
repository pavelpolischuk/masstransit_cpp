#include <masstransit_cpp/consume_context.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace masstransit_cpp
{
	consume_context_info::consume_context_info(nlohmann::json const& json)
		: message_id(boost::uuids::string_generator()(json.get<std::string>("messageId", "")))
		, conversation_id(boost::uuids::string_generator()(json.get<std::string>("conversationId", "")))
		, source_address(json.get<std::string>("sourceAddress", ""))
		, destination_address(json.get<std::string>("destinationAddress", ""))
		, message_types(json.get<std::vector<std::string>>("messageType"))
		, message(json.get<nlohmann::json::object_t>("message"))
		, send_host(json.get<host_info>("host"))
	{
	}

	consume_context_info::consume_context_info()
	{
	}

	nlohmann::json consume_context_info::to_json() const
	{
		return{ 
			{ "messageId", to_string(message_id) },
			{ "conversationId", to_string(conversation_id) },
			{ "sourceAddress", source_address },
			{ "destinationAddress", destination_address },
			{ "sourceAddress", source_address },
			{ "messageType", message_types },
			{ "message", message },
			{ "host", send_host.to_json() }
		};
	}
}
