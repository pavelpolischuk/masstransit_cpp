#include "message_consumer_depend.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>
#include <boost/di.hpp>

namespace mtc = masstransit_cpp;

namespace masstransit_cpp_tests
{
	template<class InjectorT>
	std::shared_ptr<mtc::bus> get_bus(InjectorT const& injector)
	{
		static auto bus = mtc::bus_factory::create_using_rabbit_mq([&injector](mtc::rabbit_mq_configurator & bus_configurator)
		{
			auto host = bus_configurator.host("localhost", [](mtc::amqp_host_configurator & host_configurator)
			{
				host_configurator.username("guest");
				host_configurator.password("guest");
			});

			bus_configurator.auto_delete(true);

			bus_configurator.receive_endpoint(host, "Test.AppName.Di", [&injector](mtc::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
			{
				endpoint_configurator.load_from<message_mock>(injector);
				endpoint_configurator.timeout(std::chrono::milliseconds(100));
				endpoint_configurator.exclusive(true);
			});
		});

		return bus;
	}
	
	TEST_CASE("di_config_bus_then_send_second_message_using_context_and_receive")
	{
		auto container = boost::di::make_injector(
			boost::di::bind<mtc::i_publish_endpoint>().to([](auto const& injector) -> std::shared_ptr<mtc::i_publish_endpoint> { return get_bus(injector); }),
			boost::di::bind<mtc::bus>().to([](auto const& injector) { return get_bus(injector); }),
			boost::di::bind<mtc::message_consumer<message_mock>>().to<message_consumer_depend>()
		);

		auto bus = container.create<std::shared_ptr<mtc::bus>>();
		const auto consumer = container.create<std::shared_ptr<mtc::message_consumer<message_mock>>>();
		auto consumer_t = std::dynamic_pointer_cast<message_consumer_depend>(consumer);

		bus->start();
		
		auto published = bus->publish(message_mock(message_consumer_depend::PUBLISH_WITH_CONTEXT)).get();
		consumer_t->wait();
		
		bus->stop();

		REQUIRE(published);
		REQUIRE(consumer_t->saved_value.value_or(0) == (message_consumer_depend::PUBLISH_WITH_CONTEXT + 1));
	}
}
