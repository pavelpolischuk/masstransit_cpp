#include "error_handler_mock.hpp"
#include "message_consumer_mock.hpp"
#include "message_consumer_threads.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>

namespace masstransit_cpp_tests
{
	TEST_CASE("config_bus_then_send_message_and_receive", "[bus_config]")
	{
		SECTION("consumer")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([=](masstransit_cpp::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName.Consumer", [=](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(consumer_mock);
					endpoint_configurator.timeout(std::chrono::milliseconds(100));
					endpoint_configurator.exclusive(true);
				});
			});

			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(consumer_mock->saved_value.value_or(0) == 42);
		}

		SECTION("factory")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([=](masstransit_cpp::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName.Factory", [=](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>([=]() -> std::shared_ptr<message_consumer<message_mock>>
					{
						return consumer_mock;
					});

					endpoint_configurator.timeout(std::chrono::milliseconds(100));
					endpoint_configurator.exclusive(true);
				});
			});

			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(consumer_mock->saved_value.value_or(0) == 42);
		}

		SECTION("handler")
		{
			auto res = 0;
			auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([&res](masstransit_cpp::rabbit_mq_configurator & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName.Handler", [&res](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.handler<message_mock>([&res](consume_context<message_mock> const& context) { res = context.message.id; });
					endpoint_configurator.timeout(std::chrono::milliseconds(100));
					endpoint_configurator.exclusive(true);
				});
			});


			bus->start();
			auto published = bus->publish(message_mock(42)).get();
			bus->stop();

			REQUIRE(published);
			REQUIRE(res == 42);
		}

		SECTION("concurrency_limit")
		{
			auto consumer_mock = std::make_shared<message_consumer_threads>();
			
			auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([=](auto & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName.CL", [=](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.consumer<message_mock>(consumer_mock);
					endpoint_configurator.timeout(std::chrono::milliseconds(100));
					endpoint_configurator.exclusive(true);
					endpoint_configurator.use_concurrency_limit(3);
				});
			});


			bus->start();
			
			for(auto i = 0; i < message_consumer_threads::NEED_THREAD_COUNT; ++i)
				bus->publish(message_mock(42 + i)).get();

			bus->stop();

			consumer_mock->wait();
			REQUIRE(consumer_mock->involved_threads_count() == message_consumer_threads::NEED_THREAD_COUNT);
		}

		SECTION("throw_in_handler")
		{
			auto error_handler = std::make_shared<error_handler_mock>();
			
			auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([=](auto & bus_configurator)
			{
				const auto host = bus_configurator.host("localhost", [](auto & host_configurator)
				{
					host_configurator.username("guest");
					host_configurator.password("guest");
				});

				bus_configurator.error_handler(error_handler);
				bus_configurator.auto_delete(true);

				bus_configurator.receive_endpoint(host, "Test.AppName.ER", [=](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
				{
					endpoint_configurator.handler<message_mock>([](consume_context<message_mock> const& m)
					{
						throw std::runtime_error("error message");
					});

					endpoint_configurator.timeout(std::chrono::milliseconds(100));
					endpoint_configurator.exclusive(true);
				});
			});


			bus->start();
			
			bus->publish(message_mock(42)).get();

			bus->stop();

			REQUIRE(error_handler->saved_message.value_or("") == "error message");
		}
	}
}
