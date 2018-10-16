#include "message_mock.hpp"
#include "catch.hpp"

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
	
	TEST_CASE("consume_context_tests", "[consume_context]")
	{
		SECTION("ctor_make_context_info_from_json")
		{
			auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());

			auto context = json.get<consume_context_info>();
			boost::uuids::string_generator uuid;

			REQUIRE(context.message_id == uuid("0e020000-5d98-0015-de21-08d476a8c7bf"));
			REQUIRE(context.conversation_id == uuid("0e020000-5d98-0015-df8e-08d476a8c7bf"));
			REQUIRE(context.source_address == "rabbitmq://test-app1/bus-TEST-APP1-Test.ProcessManager-123?durable=false&autodelete=true");
			REQUIRE(context.destination_address == "rabbitmq://test-app1/Test.Domain.Contracts:HandlingRequest");
			REQUIRE(context.message_types == std::vector<std::string>({
				"urn:message:Test.Domain.Contracts:HandlingRequest",
				"urn:message:Test.DomainModel.Shared:IDomainCommand" }));

			auto message = nlohmann::json::parse("{ \"id\": 543210 }");
			REQUIRE(context.message == message);

			REQUIRE(context.send_host.machine_name == "TEST-APP1");
			REQUIRE(context.send_host.process_name == "Test.ProcessManager");
			REQUIRE(context.send_host.process_id == 200668);
			REQUIRE(context.send_host.assembly == "Test.ProcessManager");
			REQUIRE(context.send_host.assembly_version == "2.11");
			REQUIRE(context.send_host.framework_version == "4.0");
			REQUIRE(context.send_host.masstransit_version == "3.5");
			REQUIRE(context.send_host.operating_system_version == "OS");
		}

		SECTION("ctor_make_context_from_info")
		{
			auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());

			auto context_info = json.get<consume_context_info>();
			consume_context<message_mock> context(context_info, nullptr);

			REQUIRE(context.info == context_info);
			REQUIRE(context.message.id == 543210);
		}

		SECTION("static_create_make_context_info_from_message")
		{
			const message_mock message(456789);
			
			auto info = consume_context_info::create(message);

			const auto message_json = nlohmann::json::parse("{ \"id\": 456789 }");
			REQUIRE(info.message.dump(0) == message_json.dump(0));
			REQUIRE(info.message_types.size() == 1);
			REQUIRE(info.message_types.front() == "urn:message:Test.Domain.Contracts:HandlingRequest");
		}

		SECTION("context_info_to_json_return_it")
		{
			auto json = nlohmann::json::parse(message_context_data.begin(), message_context_data.end());
			
			auto context_info = json.get<consume_context_info>();
			
			REQUIRE(nlohmann::json(context_info).dump(0) == json.dump(0));
		}
	}
}
