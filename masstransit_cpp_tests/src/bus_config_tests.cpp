#include "message_consumer_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>

namespace masstransit_cpp_tests
{
	using namespace masstransit_cpp;

	TEST_CASE("bus_config_tests", "[bus_config]")
	{
		SECTION( "configure_bus" ) 
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();
			
			bus_factory::create_using_rabbit_mq([=](rabbit_mq_configurator & bus_configurator)
			{
				auto host = bus_configurator.host(amqp_uri("localhost"), [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});
				
				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName", [=](auto & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(std::static_pointer_cast<message_consumer<message_mock>, message_consumer_mock>(consumer_mock));
					endpoint_configurator.poll_timeout(boost::posix_time::millisec(300));
					endpoint_configurator.auto_delete(true);
				});
			});
    	}

		SECTION("send_message_and_receive")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = bus_factory::create_using_rabbit_mq([=](rabbit_mq_configurator & bus_configurator)
			{
				auto host = bus_configurator.host(amqp_uri("localhost"), [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName", [=](auto & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(std::static_pointer_cast<message_consumer<message_mock>, message_consumer_mock>(consumer_mock));
					endpoint_configurator.poll_timeout(boost::posix_time::seconds(2));
					endpoint_configurator.auto_delete(true);
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