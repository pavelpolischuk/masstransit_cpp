#include "message_consumer_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/in_memory/in_memory_configurator.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint_configurator.hpp>

namespace masstransit_cpp_tests
{
	namespace mtc = masstransit_cpp;

	TEST_CASE("config_in_memory_bus_then_send_message_and_receive_tests", "[bus_config]")
	{
		SECTION("consumer")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = mtc::bus_factory::create_using_in_memory([=](mtc::in_memory_configurator& bus_configurator)
			{
				bus_configurator.receive_endpoint("Test.AppName", [=](mtc::in_memory::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(consumer_mock);
				});
			});


			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(consumer_mock->saved_value.get_value_or(0) == 42);
		}
	}
}
