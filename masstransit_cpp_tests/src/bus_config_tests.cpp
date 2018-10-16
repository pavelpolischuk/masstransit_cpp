#include "message_consumer_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>

namespace masstransit_cpp_tests
{
	namespace mtc = masstransit_cpp;

	TEST_CASE("config_bus_then_send_message_and_receive_tests", "[bus_config]")
	{
		SECTION("consumer")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = mtc::bus_factory::create_using_rabbit_mq([=](mtc::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName", [=](mtc::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(consumer_mock);
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

		SECTION("factory")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = mtc::bus_factory::create_using_rabbit_mq([=](mtc::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName", [=](mtc::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>([=]() -> std::shared_ptr<message_consumer<message_mock>>
					{
						return consumer_mock;
					});

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

		SECTION("handler")
		{
			auto res = 0;
			auto bus = mtc::bus_factory::create_using_rabbit_mq([&res](mtc::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName", [&res](mtc::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.handler<message_mock>([&res](consume_context<message_mock> const& context) { res = context.message.id; });
					endpoint_configurator.poll_timeout(boost::posix_time::seconds(2));
					endpoint_configurator.auto_delete(true);
				});
			});


			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(res == 42);
		}
	}
}
