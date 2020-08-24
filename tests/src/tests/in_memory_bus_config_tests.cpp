#include "message_consumer_mock.hpp"
#include "error_handler_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/in_memory/in_memory_configurator.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint_configurator.hpp>

namespace masstransit_cpp_tests
{
	TEST_CASE("config_in_memory_bus_then_send_message_and_receive", "[bus_config]")
	{
		SECTION("consumer")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = masstransit_cpp::bus_factory::create_using_in_memory([=](masstransit_cpp::in_memory_configurator& bus_configurator)
			{
				bus_configurator.receive_endpoint("Test.AppName", [=](masstransit_cpp::in_memory::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(consumer_mock);
				});
			});


			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(consumer_mock->saved_value.value_or(0) == 42);
		}

		SECTION("throw_in_handler")
		{
			auto error_handler = std::make_shared<error_handler_mock>();
			
			auto bus = masstransit_cpp::bus_factory::create_using_in_memory([=](auto & bus_configurator)
			{
				bus_configurator.error_handler(error_handler);

				bus_configurator.receive_endpoint("Test.AppName.ER", [=](masstransit_cpp::in_memory::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.handler<message_mock>([](consume_context<message_mock> const& m)
					{
						throw std::runtime_error("error message");
					});
				});
			});


			bus->start();
			bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(error_handler->saved_message.value_or("") == "error message");
		}
	}
}
