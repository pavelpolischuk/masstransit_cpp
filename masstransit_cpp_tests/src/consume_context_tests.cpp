#include <message_mock.hpp>

#include <gtest/gtest.h>
#include <boost/uuid/string_generator.hpp>

#include <masstransit_cpp/consume_context.hpp>

namespace masstransit_cpp_tests
{
	using namespace masstransit_cpp;

	static std::string message_context_data = R"(
{
	"messageId": "0e020000-5d98-0015-de21-08d476a8c7bf", 
	"conversationId" : "0e020000-5d98-0015-df8e-08d476a8c7bf",
	"sourceAddress" : "rabbitmq://test-app1/bus-TEST-APP1-Test.ProcessManager-123?durable=false&autodelete=true",
	"destinationAddress" : "rabbitmq://test-app1/Test.Domain.Contracts:HandlingRequest",
	"messageType" : [
		"urn:message:Test.Domain.Contracts:HandlingRequest",
		"urn:message:Test.DomainModel.Shared:IDomainCommand"
	],
	"message": {
		"id": 543210
	},
	"host" : {
		"machineName": "TEST-APP1",
		"processName" : "Test.ProcessManager",
		"processId" : 200668,
		"assembly" : "Test.ProcessManager",
		"assemblyVersion" : "2.11",
		"frameworkVersion" : "4.0",
		"massTransitVersion" : "3.5",
		"operatingSystemVersion" : "OS"
	}
}
)";

	TEST(consume_context_tests, ctor_make_context_info_from_json)
	{
		auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());

		consume_context_info context(json);
		boost::uuids::string_generator uuid;

		ASSERT_EQ(context.message_id, uuid("0e020000-5d98-0015-de21-08d476a8c7bf"));
		ASSERT_EQ(context.conversation_id, uuid("0e020000-5d98-0015-df8e-08d476a8c7bf"));
		ASSERT_EQ(context.source_address, "rabbitmq://test-app1/bus-TEST-APP1-Test.ProcessManager-123?durable=false&autodelete=true");
		ASSERT_EQ(context.destination_address, "rabbitmq://test-app1/Test.Domain.Contracts:HandlingRequest");
		ASSERT_EQ(context.message_types, std::vector<std::string>({
			"urn:message:Test.Domain.Contracts:HandlingRequest",
			"urn:message:Test.DomainModel.Shared:IDomainCommand" }));

		auto message = nlohmann::json::parse("{ \"id\": 543210 }");
		ASSERT_EQ(context.message, message);

		ASSERT_EQ(context.send_host.machine_name, "TEST-APP1");
		ASSERT_EQ(context.send_host.process_name, "Test.ProcessManager");
		ASSERT_EQ(context.send_host.process_id, 200668);
		ASSERT_EQ(context.send_host.assembly, "Test.ProcessManager");
		ASSERT_EQ(context.send_host.assembly_version, "2.11");
		ASSERT_EQ(context.send_host.framework_version, "4.0");
		ASSERT_EQ(context.send_host.masstransit_version, "3.5");
		ASSERT_EQ(context.send_host.operating_system_version, "OS");
	}

	TEST(consume_context_tests, ctor_make_context_from_info)
	{
		auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());

		consume_context_info context_info(json);
		consume_context<message_mock> context(context_info);

		ASSERT_EQ(context.info, context_info);
		ASSERT_EQ(context.message.id, 543210);
	}

	TEST(consume_context_tests, static_create_make_context_info_from_message)
	{
		message_mock message(456789);
		
		auto info = consume_context_info::create(message);
		
		auto message_json = nlohmann::json::parse("{ \"id\": 456789 }");
		ASSERT_EQ(info.message.dump(0), message_json.dump(0));
		ASSERT_EQ(info.message_types.size(), 1);
		ASSERT_EQ(info.message_types.front(), "urn:message:Test.Domain.Contracts:HandlingRequest");
	}

	TEST(consume_context_tests, context_info_to_json_return_it)
	{
		auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());

		consume_context_info context(json);
		
		ASSERT_EQ(context.to_json().dump(0), json.dump(0));
	}
}